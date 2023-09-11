#include <modules/tnm067lab2/processors/hydrogengenerator.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/util/volumeramutils.h>
#include <modules/base/algorithm/dataminmax.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <modules/base/algorithm/dataminmax.h>

namespace inviwo {

const ProcessorInfo HydrogenGenerator::processorInfo_{
    "org.inviwo.HydrogenGenerator",  // Class identifier
    "Hydrogen Generator",            // Display name
    "TNM067",                        // Category
    CodeState::Stable,               // Code state
    Tags::CPU,                       // Tags
};

const ProcessorInfo HydrogenGenerator::getProcessorInfo() const { return processorInfo_; }

HydrogenGenerator::HydrogenGenerator()
    : Processor(), volume_("volume"), size_("size_", "Volume Size", 16, 4, 256) {
    addPort(volume_);
    addProperty(size_);
}

void HydrogenGenerator::process() {
    auto vol = std::make_shared<Volume>(size3_t(size_), DataFloat32::get());

    auto ram = vol->getEditableRepresentation<VolumeRAM>();
    auto data = static_cast<float*>(ram->getData());
    util::IndexMapper3D index(ram->getDimensions());

    util::forEachVoxel(*ram, [&](const size3_t& pos) {
        vec3 cartesian = idTOCartesian(pos);
        data[index(pos)] = static_cast<float>(eval(cartesian));
    });

    auto minMax = util::volumeMinMax(ram);
    vol->dataMap_.dataRange = vol->dataMap_.valueRange = dvec2(minMax.first.x, minMax.second.x);

    volume_.setData(vol);
}

vec3 HydrogenGenerator::cartesianToSpherical(vec3 cartesian) {
    vec3 sph{cartesian};

    // TASK 1: implement conversion using the equations in the lab script

    auto r = sqrt(pow(cartesian.x, 2) + pow(cartesian.y, 2) + pow(cartesian.z, 2));
    auto phi = atan2(cartesian.y, cartesian.x);
    auto theta = atan2(hypot(cartesian.x, cartesian.y), cartesian.z);

    sph.r = r;
    sph.t = theta;
    sph.p = phi;

    return sph;
}

double HydrogenGenerator::eval(vec3 cartesian) {

    vec3 sph = cartesianToSpherical(cartesian);

    auto r = sph.r;
    auto theta = sph.t;
    
    const double Z = 1;
    const double a0 = 1;

    // TASK 2: Evaluate wave function
    auto psi_1 = 1.f / (81.f * sqrt(6.f * M_PI));
    auto psi_2 = pow((Z / a0), 3.f / 2.f);
    auto psi_3 = (pow(Z, 2.f) * pow(r, 2.f)) / pow(a0, 2.f);
    auto psi_4 = exp(-Z * r / (3.f * a0));
    auto psi_5 = 3.f * pow(cos(theta), 2.f) - 1.f;

    const double density = pow(psi_1 * psi_2 * psi_3 * psi_4 * psi_5, 2.0);

    return density;
}

vec3 HydrogenGenerator::idTOCartesian(size3_t pos) {
    vec3 p(pos);
    p /= size_ - 1;
    return p * (36.0f) - 18.0f;
}

}  // namespace inviwo
