#pragma once

#include "WinImgui.hpp"

class WinImgui11 : public WinImgui
{
public:
  WinImgui11( HWND hWnd, ComPtr<ID3D11Device> pD3DDevice, ComPtr<ID3D11DeviceContext> pDeviceContext, std::filesystem::path const& iniPath );
  ~WinImgui11() override;

  void     dx11_NewFrame();
  void     dx11_RenderDrawData( ImDrawData* draw_data );

  void* createTextureRaw( uint8_t const* textureData, int width, int height, TextureFormat fmt ) override;
  void deleteTextureRaw( void* textureData ) override;

private:
  void dx11_InvalidateDeviceObjects();
  void dx11_CreateFontsTexture();
  void dx11_SetupRenderState( ImDrawData* draw_data, ID3D11DeviceContext* ctx );

private:

  ComPtr<ID3D11Device>                    md3dDevice;
  ComPtr<ID3D11DeviceContext>             md3dDeviceContext;

  int mVertexBufferSize;
  int mIndexBufferSize;

  struct VERTEX_CONSTANT_BUFFER
  {
    float   mvp[4][4];
  };

  ComPtr<ID3D11Buffer>                    mVB;
  ComPtr<ID3D11Buffer>                    mIB;
  ComPtr<ID3D11VertexShader>              mVertexShader;
  ComPtr<ID3D11InputLayout>               mInputLayout;
  ComPtr<ID3D11Buffer>                    mVertexConstantBuffer;
  ComPtr<ID3D11PixelShader>               mPixelShader;
  ComPtr<ID3D11SamplerState>              mFontSampler;
  ComPtr<ID3D11ShaderResourceView>        mFontTextureView;
  ComPtr<ID3D11RasterizerState>           mRasterizerState;
  ComPtr<ID3D11BlendState>                mBlendState;
  ComPtr<ID3D11DepthStencilState>         mDepthStencilState;
};
