// Light data constant buffer - ALWAYS bound to slot 0 ======================================================
#define MAX_LIGHTS 8

// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct MyLight
{
    float4      Position;               // 16 bytes
    //----------------------------------- (16 byte boundary)
    float4      Direction;              // 16 bytes
    //----------------------------------- (16 byte boundary)
    float4      Color;                  // 16 bytes
    //----------------------------------- (16 byte boundary)
    float       SpotAngle;              // 4 bytes
    float       ConstantAttenuation;    // 4 bytes
    float       LinearAttenuation;      // 4 bytes
    float       QuadraticAttenuation;   // 4 bytes
    //----------------------------------- (16 byte boundary)
    int         LightType;              // 4 bytes
    bool        Enabled;                // 4 bytes
    int2        Padding;                // 8 bytes
    //----------------------------------- (16 byte boundary)
};  // Total:                           // 80 bytes (5 * 16 byte boundary)

cbuffer MyLightProperties : register(b0)
{
    float4 EyePosition;                 // 16 bytes
    //----------------------------------- (16 byte boundary)
    float4 GlobalAmbient;               // 16 bytes
    //----------------------------------- (16 byte boundary)
    MyLight Lights[MAX_LIGHTS];           // 80 * 8 = 640 bytes
};  // Total:                           // 672 bytes (42 * 16 byte boundary)
