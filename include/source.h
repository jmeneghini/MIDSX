#ifdef SOURCE_H
#define SOURCE_H

#include "vec3.h"

class Source {
public:
    Source() {}
    Source(const Vec3 sourcePosition, const Spec sourceSpectrum, const float detectorDistance, const float detectorWidth, const float detectorHeight) : sourcePosition(sourcePosition), sourceSpectrum(sourceSpectrum), detectorDistance(detectorDistance), detectorWidth(detectorWidth), detectorHeight(detectorHeight) {
        sceneOrigin = Vec3(0, 0, 0);
        detectorHorizontal = Vec3(detectorWidth, 0, 0);
        detectorVertical = Vec3(0, detectorHeight, 0);
        detectorLowerLeftCorner = sceneOrigin - detectorHorizontal/2 - detectorVertical/2 - Vec3(0, 0, detectorDistance);
    }

    Photon emitPhoton();

private:
    Vec3 sourcePosition;
    Spec sourceSpectrum;
    float detectorDistance;
    float detectorWidth;
    float detectorHeight;
    Vec3 sceneOrigin;
    Vec3 detectorHorizontal;
    Vec3 detectorVertical;
    Vec3 detectorLowerLeftCorner;
};

#endif
    

