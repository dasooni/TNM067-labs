#include <inviwo/tnm067lab3/processors/marchingtetrahedra.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/network/networklock.h>
#include <modules/tnm067lab1/utils/interpolationmethods.h>
#include <iostream>
#include <fstream>

namespace inviwo {

size_t MarchingTetrahedra::HashFunc::max = 1;

const ProcessorInfo MarchingTetrahedra::processorInfo_{
    "org.inviwo.MarchingTetrahedra",  // Class identifier
    "Marching Tetrahedra",            // Display name
    "TNM067",                         // Category
    CodeState::Stable,                // Code state
    Tags::None,                       // Tags
};
const ProcessorInfo MarchingTetrahedra::getProcessorInfo() const { return processorInfo_; }

MarchingTetrahedra::MarchingTetrahedra()
    : Processor()
    , volume_("volume")
    , mesh_("mesh")
    , isoValue_("isoValue", "ISO value", 0.5f, 0.0f, 1.0f) {

    addPort(volume_);
    addPort(mesh_);

    addProperty(isoValue_);

    isoValue_.setSerializationMode(PropertySerializationMode::All);

    volume_.onChange([&]() {
        if (!volume_.hasData()) {
            return;
        }
        NetworkLock lock(getNetwork());
        float iso = (isoValue_.get() - isoValue_.getMinValue()) /
                    (isoValue_.getMaxValue() - isoValue_.getMinValue());
        const auto vr = volume_.getData()->dataMap_.valueRange;
        isoValue_.setMinValue(static_cast<float>(vr.x));
        isoValue_.setMaxValue(static_cast<float>(vr.y));
        isoValue_.setIncrement(static_cast<float>(glm::abs(vr.y - vr.x) / 50.0));
        isoValue_.set(static_cast<float>(iso * (vr.y - vr.x) + vr.x));
        isoValue_.setCurrentStateAsDefault();
    });
}

void MarchingTetrahedra::process() {
    auto volume = volume_.getData()->getRepresentation<VolumeRAM>();
    MeshHelper mesh(volume_.getData());

    const auto& dims = volume->getDimensions();
    MarchingTetrahedra::HashFunc::max = dims.x * dims.y * dims.z;

    const float iso = isoValue_.get();

    util::IndexMapper3D mapVolPosToIndex(dims);

    const static size_t tetrahedraIds[6][4] = {{0, 1, 2, 5}, {1, 3, 2, 5}, {3, 2, 5, 7},
                                               {0, 2, 4, 5}, {6, 4, 2, 5}, {6, 7, 5, 2}};

    size3_t pos{};
    for (pos.z = 0; pos.z < dims.z - 1; ++pos.z) {
        for (pos.y = 0; pos.y < dims.y - 1; ++pos.y) {
            for (pos.x = 0; pos.x < dims.x - 1; ++pos.x) {
                // The DataPoint index should be the 1D-index for the DataPoint in the cell
                // Use volume->getAsDouble to query values from the volume
                // Spatial position should be between 0 and 1

                // TODO: TASK 2: create a nested for loop to construct the cell
                Cell c{};

                // Use calculateDataPointIndexInCell(vec3 index3D) and calculateDataPointPos(posVolume, posCell, dims)

                for (auto i = 0; i < 2; i++) {
                    for (auto j = 0; j < 2; j++) {
                        for (auto k = 0; k < 2; k++) {
                            vec3 posCell = vec3(k, j, i);

                            auto scaledCellPos = calculateDataPointPos(pos, posCell, dims);
                            int cellIndex = calculateDataPointIndexInCell(posCell);
                            auto cellVal = volume->getAsDouble(vec3{pos.x + k, pos.y + j, pos.z + i});

                            c.dataPoints[cellIndex].pos = scaledCellPos;
                            c.dataPoints[cellIndex].value = cellVal;
                            c.dataPoints[cellIndex].indexInVolume =
                                mapVolPosToIndex(vec3{pos.x + k, pos.y + j, pos.z + i});
                        }
                    }
                
                }
                // TODO: TASK 3: Subdivide cell into 6 tetrahedra (hint: use tetrahedraIds)
                std::vector<Tetrahedra> tetrahedras;

                Tetrahedra t;

                for (auto i = 0; i < 6; i++) {
                    for (auto j = 0; j < 4; j++) {
						t.dataPoints[j] = c.dataPoints[tetrahedraIds[i][j]];
					}
					tetrahedras.push_back(t);
				}

                for (const Tetrahedra& tetrahedra : tetrahedras) {
                    // TODO: TASK 4: Calculate case id for each tetrahedra, and add triangles for
                    // each case (use MeshHelper)


                    // Calculate for tetra case index
                    int caseId = 0;

                    for (auto i = 0; i < 4; i++) {
                        if (tetrahedra.dataPoints[i].value < iso) {
                            caseId += pow(2, i);
                        }
                    }

                    auto i1 = tetrahedra.dataPoints[0].indexInVolume;
                    auto i2 = tetrahedra.dataPoints[1].indexInVolume;
                    auto i3 = tetrahedra.dataPoints[2].indexInVolume;
                    auto i4 = tetrahedra.dataPoints[3].indexInVolume;

                    auto v1 = tetrahedra.dataPoints[0].value;
                    auto v2 = tetrahedra.dataPoints[1].value;
                    auto v3 = tetrahedra.dataPoints[2].value;
                    auto v4 = tetrahedra.dataPoints[3].value;

                    auto p1 = tetrahedra.dataPoints[0].pos;
                    auto p2 = tetrahedra.dataPoints[1].pos;
                    auto p3 = tetrahedra.dataPoints[2].pos;
                    auto p4 = tetrahedra.dataPoints[3].pos;
                    
                    // Extract triangles
                    switch (caseId) {
                        case 0:
                        case 15:
                            break;
                        case 1:
                        case 14: {

                            vec3 interp1 = p1 + (p2 - p1) * (iso - v1) / (v2 - v1);
                            vec3 interp2 = p1 + (p4 - p1) * (iso - v1) / (v4 - v1);
                            vec3 interp3 = p1 + (p3 - p1) * (iso - v1) / (v3 - v1);

                            auto t1 = mesh.addVertex(interp1, i1, i2);
                            auto t2 = mesh.addVertex(interp2, i1, i4);
                            auto t3 = mesh.addVertex(interp3, i1, i3);

                            if (caseId == 14) {
                                mesh.addTriangle(t1, t2, t3);
                                
                            } else {
                                mesh.addTriangle(t1, t3, t2);
                            }
                            break;
                        }
                        case 2:
                        case 13: {

                            vec3 interp1 = p2 + (p4 - p2) * (iso - v2) / (v4 - v2);
                            vec3 interp2 = p2 + (p3 - p2) * (iso - v2) / (v3 - v2);
                            vec3 interp3 = p2 + (p1 - p2) * (iso - v2) / (v1 - v2);

                            auto t1 = mesh.addVertex(interp1, i2, i4);
                            auto t2 = mesh.addVertex(interp2, i2, i3);
                            auto t3 = mesh.addVertex(interp3, i2, i1);

                            if (caseId == 2) {
                                mesh.addTriangle(t1, t2, t3);

                            } else {
                                mesh.addTriangle(t1, t3, t2);
                            }
                            break;

                        }
                        case 3:
                        case 12: {

                            vec3 interp1 = p2 + (p3 - p2) * (iso - v2) / (v3 - v2);
                            vec3 interp2 = p2 + (p4 - p2) * (iso - v2) / (v4 - v2);
                            vec3 interp3 = p1 + (p4 - p1) * (iso - v1) / (v4 - v1);
                            vec3 interp4 = p1 + (p3 - p1) * (iso - v1) / (v3 - v1);

                            auto t1 = mesh.addVertex(interp1, i2, i3);
                            auto t2 = mesh.addVertex(interp2, i2, i4);
                            auto t3 = mesh.addVertex(interp3, i1, i4);
                            auto t4 = mesh.addVertex(interp4, i1, i3);


                            if (caseId == 3) {
                                mesh.addTriangle(t1, t3, t2);
                                mesh.addTriangle(t1, t4, t3);

                            } else {
                                mesh.addTriangle(t3, t1, t2);
                                mesh.addTriangle(t4, t1, t3);
                            }

                            break;
                        }
                        case 4:
                        case 11: {

                            vec3 interp1 = p3 + (p1 - p3) * (iso - v3) / (v1 - v3);
                            vec3 interp2 = p3 + (p2 - p3) * (iso - v3) / (v2 - v3);
                            vec3 interp3 = p3 + (p4 - p3) * (iso - v3) / (v4 - v3);

                            auto t1 = mesh.addVertex(interp1, i3, i1);
                            auto t2 = mesh.addVertex(interp2, i3, i2);
                            auto t3 = mesh.addVertex(interp3, i3, i4);

                            if (caseId == 4) {
                                mesh.addTriangle(t1, t2, t3);

                            } else {
                                mesh.addTriangle(t1, t3, t2);
                            }

                            break;
                        }
                        case 5:
                        case 10: {

                            vec3 interp1 = p1 + (p4 - p1) * (iso - v1) / (v4 - v1);
                            vec3 interp2 = p1 + (p2 - p1) * (iso - v1) / (v2 - v1);
                            vec3 interp3 = p3 + (p2 - p3) * (iso - v3) / (v2 - v3);
                            vec3 interp4 = p3 + (p4 - p3) * (iso - v3) / (v4 - v3);

                            auto t1 = mesh.addVertex(interp1, i1, i4);
                            auto t2 = mesh.addVertex(interp2, i1, i2);
                            auto t3 = mesh.addVertex(interp3, i3, i2);
                            auto t4 = mesh.addVertex(interp4, i3, i4);

                            if (caseId == 5) {
                                mesh.addTriangle(t1, t2, t3);
                                mesh.addTriangle(t1, t3, t4);

                            } else {
                                mesh.addTriangle(t3, t2, t1);
                                mesh.addTriangle(t4, t3, t1);
                            }

                            break;
                        }
                        case 6:
                        case 9: {

                            vec3 interp1 = p1 + (p3 - p1) * (iso - v1) / (v3 - v1);
                            vec3 interp2 = p1 + (p2 - p1) * (iso - v1) / (v2 - v1);
                            vec3 interp3 = p2 + (p4 - p2) * (iso - v2) / (v4 - v2);
                            vec3 interp4 = p3 + (p4 - p3) * (iso - v3) / (v4 - v3);

                            auto t1 = mesh.addVertex(interp1, i1, i3);
                            auto t2 = mesh.addVertex(interp2, i1, i2);
                            auto t3 = mesh.addVertex(interp3, i2, i4);
                            auto t4 = mesh.addVertex(interp4, i3, i4);

                            if (caseId == 9) {
                                mesh.addTriangle(t1, t3, t2);
                                mesh.addTriangle(t1, t4, t3);

                            } else {
                                mesh.addTriangle(t1, t2, t3);
                                mesh.addTriangle(t1, t3, t4);
                            }

                            break;
                        }
                        case 7:
                        case 8: {
                            vec3 interp1 = p4 + (p1 - p4) * (iso - v4) / (v1 - v4);
                            vec3 interp2 = p4 + (p2 - p4) * (iso - v4) / (v2 - v4);
                            vec3 interp3 = p4 + (p3 - p4) * (iso - v4) / (v3 - v4);

                            auto t1 = mesh.addVertex(interp1, i4, i1);
                            auto t2 = mesh.addVertex(interp2, i4, i2);
                            auto t3 = mesh.addVertex(interp3, i4, i3);

                            if (caseId == 8) {
                                mesh.addTriangle(t1, t3, t2);

                            } else {
                                mesh.addTriangle(t1, t2, t3);
                            }                                                                               
                            break;
                        }
                    }
                }
            }
        }
    }

    mesh_.setData(mesh.toBasicMesh());
}

int MarchingTetrahedra::calculateDataPointIndexInCell(ivec3 index3D) {
    // index3D: 3D-index of the data point in the cell

    int size = 2;
    // https://stackoverflow.com/questions/21596373/compute-shaders-input-3d-array-of-floats
    return index3D.x + index3D.y * size + index3D.z * size * size;
}

vec3 MarchingTetrahedra::calculateDataPointPos(size3_t posVolume, ivec3 posCell, ivec3 dims) {
    // posVolume: position of the cell in the volume
    // posCell: 3D-index of the data point in the cell
    // dims: dimensions of the volume
    // 
    // This function should return the position of the data point within the volume scaled
    // between 0 and 1

    vec3 normalizedPos{};

    normalizedPos.x = static_cast<float>(posVolume.x + posCell.x) / (dims.x - 1);
    normalizedPos.y = static_cast<float>(posVolume.y + posCell.y) / (dims.y - 1);
    normalizedPos.z = static_cast<float>(posVolume.z + posCell.z) / (dims.z - 1);

    return normalizedPos;
}

MarchingTetrahedra::MeshHelper::MeshHelper(std::shared_ptr<const Volume> vol)
    : edgeToVertex_()
    , vertices_()
    , mesh_(std::make_shared<BasicMesh>())
    , indexBuffer_(mesh_->addIndexBuffer(DrawType::Triangles, ConnectivityType::None)) {
    mesh_->setModelMatrix(vol->getModelMatrix());
    mesh_->setWorldMatrix(vol->getWorldMatrix());
}

void MarchingTetrahedra::MeshHelper::addTriangle(size_t i0, size_t i1, size_t i2) {
    IVW_ASSERT(i0 != i1, "i0 and i1 should not be the same value");
    IVW_ASSERT(i0 != i2, "i0 and i2 should not be the same value");
    IVW_ASSERT(i1 != i2, "i1 and i2 should not be the same value");

    

    indexBuffer_->add(static_cast<glm::uint32_t>(i0));
    indexBuffer_->add(static_cast<glm::uint32_t>(i1));
    indexBuffer_->add(static_cast<glm::uint32_t>(i2));

    const auto a = std::get<0>(vertices_[i0]);
    const auto b = std::get<0>(vertices_[i1]);
    const auto c = std::get<0>(vertices_[i2]);

    const vec3 n = glm::normalize(glm::cross(b - a, c - a));
    std::get<1>(vertices_[i0]) += n;
    std::get<1>(vertices_[i1]) += n;
    std::get<1>(vertices_[i2]) += n;
}

std::shared_ptr<BasicMesh> MarchingTetrahedra::MeshHelper::toBasicMesh() {
    for (auto& vertex : vertices_) {
        // Normalize the normal of the vertex
        std::get<1>(vertex) = glm::normalize(std::get<1>(vertex));
    }
    mesh_->addVertices(vertices_);
    return mesh_;
}

std::uint32_t MarchingTetrahedra::MeshHelper::addVertex(vec3 pos, size_t i, size_t j) {
    IVW_ASSERT(i != j, "i and j should not be the same value");
    if (j < i) std::swap(i, j);

    auto [edgeIt, inserted] = edgeToVertex_.try_emplace(std::make_pair(i, j), vertices_.size());
    if (inserted) {
        vertices_.push_back({pos, vec3(0, 0, 0), pos, vec4(0.7f, 0.7f, 0.7f, 1.0f)});
    }
    return static_cast<std::uint32_t>(edgeIt->second);
}

}  // namespace inviwo
