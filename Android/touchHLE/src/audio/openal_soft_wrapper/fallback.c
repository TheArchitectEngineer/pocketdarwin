#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef char ALboolean;
typedef char ALchar;
typedef int ALenum;
typedef int ALint;
typedef int ALsizei;
typedef unsigned int ALuint;
typedef float ALfloat;
typedef void ALvoid;

typedef char ALCboolean;
typedef char ALCchar;
typedef int ALCenum;
typedef int ALCint;
typedef void ALCcontext;
typedef void ALCdevice;

static ALuint g_next_id = 1;
static const ALchar *kVendor = "touchHLE fallback";
static const ALchar *kVersion = "0.0";
static const ALchar *kRenderer = "No OpenAL backend";
static const ALchar *kExtensions = "";
static const ALchar *kDevice = "Fallback Device";

ALCdevice *alcOpenDevice(const ALCchar *devicename) {
    (void)devicename;
    return (ALCdevice *)1;
}
ALCboolean alcCloseDevice(ALCdevice *device) {
    (void)device;
    return 1;
}
ALCcontext *alcCreateContext(ALCdevice *device, const ALCint *attrlist) {
    (void)device;
    (void)attrlist;
    return (ALCcontext *)1;
}
void alcDestroyContext(ALCcontext *context) {
    (void)context;
}
void alcProcessContext(ALCcontext *context) {
    (void)context;
}
void alcSuspendContext(ALCcontext *context) {
    (void)context;
}
ALCboolean alcMakeContextCurrent(ALCcontext *context) {
    (void)context;
    return 1;
}
ALCcontext *alcGetCurrentContext(void) {
    return (ALCcontext *)1;
}
ALCdevice *alcGetContextsDevice(ALCcontext *context) {
    (void)context;
    return (ALCdevice *)1;
}
ALCenum alcGetError(ALCdevice *device) {
    (void)device;
    return 0;
}
const ALCchar *alcGetString(ALCdevice *device, ALCenum param) {
    (void)device;
    (void)param;
    return kDevice;
}

ALenum alGetError(void) {
    return 0;
}
void alDistanceModel(ALenum value) {
    (void)value;
}
ALenum alGetEnumValue(const ALchar *enumName) {
    (void)enumName;
    return 0;
}
ALboolean alIsBuffer(ALuint buffer) {
    (void)buffer;
    return 1;
}
ALboolean alIsSource(ALuint source) {
    (void)source;
    return 1;
}
void alEnable(ALenum capability) {
    (void)capability;
}
void alGetBufferi(ALuint buffer, ALenum param, const ALint *value) {
    (void)buffer;
    (void)param;
    (void)value;
}
void alListenerf(ALenum param, ALfloat value) {
    (void)param;
    (void)value;
}
void alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3) {
    (void)param;
    (void)value1;
    (void)value2;
    (void)value3;
}
void alListenerfv(ALenum param, const ALfloat *values) {
    (void)param;
    (void)values;
}
void alListeneri(ALenum param, ALint value) {
    (void)param;
    (void)value;
}
void alListener3i(ALenum param, ALint value1, ALint value2, ALint value3) {
    (void)param;
    (void)value1;
    (void)value2;
    (void)value3;
}
void alListeneriv(ALenum param, const ALint *values) {
    (void)param;
    (void)values;
}
void alGetListenerf(ALenum param, ALfloat *value) {
    (void)param;
    if (value != NULL) {
        *value = 0.0f;
    }
}
void alGetListener3f(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3) {
    (void)param;
    if (value1 != NULL) *value1 = 0.0f;
    if (value2 != NULL) *value2 = 0.0f;
    if (value3 != NULL) *value3 = 0.0f;
}
void alGetListenerfv(ALenum param, ALfloat *values) {
    (void)param;
    if (values != NULL) {
        values[0] = 0.0f;
    }
}
void alGetListeneri(ALenum param, ALint *value) {
    (void)param;
    if (value != NULL) {
        *value = 0;
    }
}
void alGetListener3i(ALenum param, ALint *value1, ALint *value2, ALint *value3) {
    (void)param;
    if (value1 != NULL) *value1 = 0;
    if (value2 != NULL) *value2 = 0;
    if (value3 != NULL) *value3 = 0;
}
void alGetListeneriv(ALenum param, ALint *values) {
    (void)param;
    if (values != NULL) {
        values[0] = 0;
    }
}
void alGenSources(ALsizei n, ALuint *sources) {
    for (ALsizei i = 0; i < n; i++) {
        sources[i] = g_next_id++;
    }
}
void alDeleteSources(ALsizei n, const ALuint *sources) {
    (void)n;
    (void)sources;
}
void alSourcef(ALuint source, ALenum param, ALfloat value) {
    (void)source;
    (void)param;
    (void)value;
}
void alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3) {
    (void)source;
    (void)param;
    (void)value1;
    (void)value2;
    (void)value3;
}
void alSourcefv(ALuint source, ALenum param, const ALfloat *values) {
    (void)source;
    (void)param;
    (void)values;
}
void alSourcei(ALuint source, ALenum param, ALint value) {
    (void)source;
    (void)param;
    (void)value;
}
void alSource3i(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3) {
    (void)source;
    (void)param;
    (void)value1;
    (void)value2;
    (void)value3;
}
void alSourceiv(ALuint source, ALenum param, const ALint *values) {
    (void)source;
    (void)param;
    (void)values;
}
void alGetSourcef(ALuint source, ALenum param, ALfloat *value) {
    (void)source;
    (void)param;
    if (value != NULL) {
        *value = 0.0f;
    }
}
void alGetSource3f(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3) {
    (void)source;
    (void)param;
    if (value1 != NULL) *value1 = 0.0f;
    if (value2 != NULL) *value2 = 0.0f;
    if (value3 != NULL) *value3 = 0.0f;
}
void alGetSourcefv(ALuint source, ALenum param, ALfloat *values) {
    (void)source;
    (void)param;
    if (values != NULL) {
        values[0] = 0.0f;
    }
}
void alGetSourcei(ALuint source, ALenum param, ALint *value) {
    (void)source;
    if (value == NULL) {
        return;
    }
    switch (param) {
        case 0x1010:
            *value = 0x1014;
            break;
        default:
            *value = 0;
            break;
    }
}
void alGetSource3i(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3) {
    (void)source;
    (void)param;
    if (value1 != NULL) *value1 = 0;
    if (value2 != NULL) *value2 = 0;
    if (value3 != NULL) *value3 = 0;
}
void alGetSourceiv(ALuint source, ALenum param, ALint *values) {
    (void)source;
    if (values == NULL) {
        return;
    }
    switch (param) {
        case 0x1010:
            values[0] = 0x1014;
            break;
        case 0x1015:
        case 0x1016:
            values[0] = 0;
            break;
        default:
            values[0] = 0;
            break;
    }
}
void alSourcePlay(ALuint source) {
    (void)source;
}
void alSourcePause(ALuint source) {
    (void)source;
}
void alSourceStop(ALuint source) {
    (void)source;
}
void alSourceRewind(ALuint source) {
    (void)source;
}
void alSourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers) {
    (void)source;
    (void)nb;
    (void)buffers;
}
void alSourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers) {
    (void)source;
    if (buffers != NULL) {
        memset(buffers, 0, (size_t)nb * sizeof(ALuint));
    }
}
void alGenBuffers(ALsizei n, ALuint *buffers) {
    for (ALsizei i = 0; i < n; i++) {
        buffers[i] = g_next_id++;
    }
}
void alDeleteBuffers(ALsizei n, const ALuint *buffers) {
    (void)n;
    (void)buffers;
}
void alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei samplerate) {
    (void)buffer;
    (void)format;
    (void)data;
    (void)size;
    (void)samplerate;
}
void alDopplerFactor(ALfloat dopplerFactor) {
    (void)dopplerFactor;
}
void alDopplerVelocity(ALfloat dopplerVelocity) {
    (void)dopplerVelocity;
}
void alSpeedOfSound(ALfloat speed) {
    (void)speed;
}

const ALchar *alGetString(ALenum param) {
    switch (param) {
        case 0xb001:
            return kVendor;
        case 0xb002:
            return kVersion;
        case 0xb003:
            return kRenderer;
        case 0xb004:
            return kExtensions;
        default:
            return "";
    }
}
