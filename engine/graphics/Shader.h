#pragma once

// #include "Core/Types.h"
// #include "Core/IResource.h"
// #include <functional>

// struct ID3D11DeviceContext; //should be abstracted

// // using FSetShader = std::function<void(void*, ID3D11DeviceContext*)>;

// namespace Graphics
// {
//     class Shader : public IResource
//     {
//     public:
//         Shader(FSetShader setShaderFnc, void* pShader);
//         ~Shader();
//         void Bind(ID3D11DeviceContext* deviceContext);
//     private: 
//         FSetShader m_Callback;
//         void* m_Shader = nullptr;
//     };
// }; //namespace Graphics