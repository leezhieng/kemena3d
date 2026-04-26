/**
 * @file kaudiosource.h
 * @brief Audio emitter and listener component descriptors for scene objects.
 */

#ifndef KAUDIOSOURCE_H
#define KAUDIOSOURCE_H

#include "kexport.h"
#include "kdatatype.h"

namespace kemena
{
    /**
     * @brief Describes an audio emitter attached to a scene object.
     *
     * At runtime kAudioManager loads the referenced file and positions
     * the sound at the owning object's world-space position each frame.
     */
    struct KEMENA3D_API kAudioSource
    {
        kString uuid;
        kString name        = "Audio Source";
        kString audioFile   = "";    ///< Path to the audio clip (WAV / MP3 / OGG / FLAC).
        bool    isActive    = true;
        bool    playOnAwake = false;
        bool    loop        = false;
        float   volume      = 1.0f;  ///< 0 = silent, 1 = full volume.
        float   pitch       = 1.0f;  ///< 1 = normal speed/pitch.
        bool    spatialize  = true;  ///< false = 2-D (no panning/attenuation).
        float   minDistance = 1.0f;  ///< Distance at which attenuation begins.
        float   maxDistance = 100.0f;///< Distance beyond which the sound is inaudible.
    };

    /**
     * @brief Marks a scene object as the audio listener (typically the main camera).
     *
     * The runtime queries objects carrying a kAudioListener each frame and
     * forwards their position and orientation to kAudioManager::setListenerPosition().
     * Only one active listener per scene is meaningful.
     */
    struct KEMENA3D_API kAudioListener
    {
        kString uuid;
        bool    isActive = true;
    };

} // namespace kemena

#endif // KAUDIOSOURCE_H
