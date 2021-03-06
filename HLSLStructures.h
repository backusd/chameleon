#pragma once
#include "pch.h"

constexpr auto MAX_LIGHTS = 8;

struct ModelViewProjectionConstantBuffer
{
    DirectX::XMFLOAT4X4 model;
    DirectX::XMFLOAT4X4 modelViewProjection;
    DirectX::XMFLOAT4X4 inverseTransposeModel;
};

struct VertexPositionNormal
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
};

struct _PhongMaterial
{
    _PhongMaterial()
        : Emissive(0.0f, 0.0f, 0.0f, 1.0f)
        , Ambient(0.1f, 0.1f, 0.1f, 1.0f)
        , Diffuse(1.0f, 1.0f, 1.0f, 1.0f)
        , Specular(1.0f, 1.0f, 1.0f, 1.0f)
        , SpecularPower(128.0f)
        , UseTexture(false)
    {}

    DirectX::XMFLOAT4   Emissive;
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4   Ambient;
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4   Diffuse;
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4   Specular;
    //----------------------------------- (16 byte boundary)
    float               SpecularPower;
    // Add some padding complete the 16 byte boundary.
    int                 UseTexture;
    // Add some padding to complete the 16 byte boundary.
    float               Padding[2];
    //----------------------------------- (16 byte boundary)
}; // Total:                                80 bytes (5 * 16)

struct PhongMaterialProperties
{
    _PhongMaterial   Material;
};

struct SolidMaterialProperties
{
    DirectX::XMFLOAT4   Color;
};

enum LightType
{
    DirectionalLight = 0,
    PointLight = 1,
    SpotLight = 2
};

struct Light
{
    Light()
        : Position(0.0f, 0.0f, 0.0f, 1.0f)
        , Direction(0.0f, 0.0f, 1.0f, 0.0f)
        , Color(1.0f, 1.0f, 1.0f, 1.0f)
        , SpotAngle(DirectX::XM_PIDIV2)
        , ConstantAttenuation(1.0f)
        , LinearAttenuation(0.0f)
        , QuadraticAttenuation(0.0f)
        , LightType(DirectionalLight)
        , Enabled(0)
    {}

    DirectX::XMFLOAT4    Position;
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4    Direction;
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4    Color;
    //----------------------------------- (16 byte boundary)
    float       SpotAngle;
    float       ConstantAttenuation;
    float       LinearAttenuation;
    float       QuadraticAttenuation;
    //----------------------------------- (16 byte boundary)
    int         LightType;
    int         Enabled;
    // Add some padding to make this struct size a multiple of 16 bytes.
    int         Padding[2];
    //----------------------------------- (16 byte boundary)
};  // Total:                              80 bytes ( 5 * 16 )

struct LightProperties
{
    LightProperties()
        : EyePosition(0.0f, 0.0f, 0.0f, 1.0f)
        , GlobalAmbient(0.2f, 0.2f, 0.8f, 1.0f)
    {}

    DirectX::XMFLOAT4   EyePosition;
    //----------------------------------- (16 byte boundary)
    DirectX::XMFLOAT4   GlobalAmbient;
    //----------------------------------- (16 byte boundary)
    Light               Lights[MAX_LIGHTS]; // 80 * 8 bytes
};  // Total:                                  672 bytes (42 * 16)

struct PhongPSConfigurationData
{
    BOOL normalMapEnabled;      // Use 4-byte boolean to align with pixel shader constant buffer layout
    BOOL specularMapEnabled;
    float specularIntensity;
    float specularPower;
};

// Terrain ===========================================================
struct TerrainVertexType
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 texture;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT3 binormal;
    DirectX::XMFLOAT3 color;
};

struct TerrainLightBufferType
{
    DirectX::XMFLOAT4 diffuseColor;
    DirectX::XMFLOAT3 lightDirection;
    float padding;
};

// Sky Dome ==============================================================
struct SkyDomeVertexType
{
    DirectX::XMFLOAT3 position;
};

struct SkyDomeColorBufferType
{
    DirectX::XMFLOAT4 apexColor;
    DirectX::XMFLOAT4 centerColor;
};

// Solid Color ==========================================================
struct SolidColorVertexType
{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 color;
};