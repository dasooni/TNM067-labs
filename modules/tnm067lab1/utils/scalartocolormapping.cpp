#include <modules/tnm067lab1/utils/scalartocolormapping.h>

namespace inviwo {

void ScalarToColorMapping::clearColors() { baseColors_.clear(); }
void ScalarToColorMapping::addBaseColors(vec4 color) { baseColors_.push_back(color); }

vec4 ScalarToColorMapping::sample(float t) const {
    if (baseColors_.size() == 0) return vec4(t); // if no colors in baseColors_ return t
    if (baseColors_.size() == 1) return vec4(baseColors_[0]); // if only one color in baseColors_ return that color
    if (t <= 0) return vec4(baseColors_.front()); // if t is less than/equal to 0 return first color in baseColors_
    if (t >= 1) return vec4(baseColors_.back()); // if t is greater/equal to 1 return last color in baseColors_

    // Calculate which two colors (red & blue) from baseColors_ that t is between 
    float color_ = t * (baseColors_.size() - 1.0f); // color_ is the index of the color in baseColors_ that t is between
    // t is the pixel value. baseColors_.size() is the number of colors. 
    // Thus, t * (baseColors_.size() - 1.0f) is the index of the color in baseColors_ that t is between

    vec4 leftColor = baseColors_[static_cast<int>(color_)]; // start color
    vec4 rightColor = baseColors_[static_cast<int>(color_) + 1.0f];  // end color

    // TODO: Interpolate colors in baseColors_ and set dummy color to result
    color_ -= static_cast<int>(color_); // color_ is now the interpolation factor between start and end color
    // by subtracting the integer part of the color_ we get a value that is between leftColor and rightColor??

    vec4 finalColor =
        (1 - color_) * leftColor + color_ * rightColor;  // interpolate between start and end color 
       
    return finalColor;
}

}  // namespace inviwo
