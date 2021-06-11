#include "pch.hpp"
#include "WinAudioOut.hpp"
#include "Log.hpp"

WinAudioOut::WinAudioOut( uint32_t audioBufferLengthMs )
{
  CoInitializeEx( NULL, COINIT_MULTITHREADED );

  HRESULT hr;

  ComPtr<IMMDeviceEnumerator> deviceEnumerator;
  hr = CoCreateInstance( __uuidof( MMDeviceEnumerator ), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( deviceEnumerator.ReleaseAndGetAddressOf() ) );
  if ( FAILED( hr ) )
    throw std::exception{};

  hr = deviceEnumerator->GetDefaultAudioEndpoint( eRender, eMultimedia, mDevice.ReleaseAndGetAddressOf() );
  if ( FAILED( hr ) )
    throw std::exception{};

  hr = mDevice->Activate( __uuidof( IAudioClient ), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void **>( mAudioClient.ReleaseAndGetAddressOf() ) );
  if ( FAILED( hr ) )
    throw std::exception{};

  hr = mAudioClient->GetMixFormat( &mMixFormat );
  if ( FAILED( hr ) )
    throw std::exception{};

  hr = mAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 0, 0, mMixFormat, nullptr );
  if ( FAILED( hr ) )
    throw std::exception{};

  mEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
  if ( mEvent == NULL )
    throw std::exception{};

  mAudioClient->SetEventHandle( mEvent );
  if ( FAILED( hr ) )
    throw std::exception{};

  hr = mAudioClient->GetBufferSize( &mBufferSize );
  if ( FAILED( hr ) )
    throw std::exception{};

  hr = mAudioClient->GetService( __uuidof( IAudioRenderClient ), reinterpret_cast<void **>( mRenderClient.ReleaseAndGetAddressOf() ) );
  if ( FAILED( hr ) )
    throw std::exception{};

  hr = mAudioClient->GetService( __uuidof( IAudioClock ), reinterpret_cast<void **>( mAudioClock.ReleaseAndGetAddressOf() ) );
  if ( FAILED( hr ) )
    throw std::exception{};

  hr = mAudioClock->GetFrequency( &mDeviceFrequency );
  if ( FAILED( hr ) )
    throw std::exception{};

  mAudioClient->Start();
}

WinAudioOut::~WinAudioOut()
{
  mAudioClient->Stop();

  if ( mEvent )
  {
    CloseHandle( mEvent );
    mEvent = NULL;
  }

  CoUninitialize();

  if ( mMixFormat )
  {
    CoTaskMemFree( mMixFormat );
    mMixFormat = nullptr;
  }
}

void WinAudioOut::fillBuffer( std::function<std::pair<float, float>( int sps )> & fun )
{
  DWORD retval = WaitForSingleObject( mEvent, 2000 );
  if ( retval != WAIT_OBJECT_0 )
    return;

  HRESULT hr;
  uint32_t padding{};
  hr = mAudioClient->GetCurrentPadding( &padding );
  uint32_t framesAvailable = mBufferSize - padding;
  L_TRACE << "AUD " << mBufferSize << " - " << padding << " = " << framesAvailable;
  if ( framesAvailable > 0 )
  {
    BYTE *pData;
    hr = mRenderClient->GetBuffer( framesAvailable, &pData );
    if ( FAILED( hr ) )
      return;
    float* pfData = reinterpret_cast<float*>( pData );
    for ( uint32_t i = 0; i < framesAvailable; ++i )
    {
      auto pair = fun( mMixFormat->nSamplesPerSec );
      for ( int j = 0; j < mMixFormat->nChannels; ++j )
        pfData[i * mMixFormat->nChannels + j] = ( ( j & 1 ) == 0 ) ? pair.first : pair.second;
    }
    hr = mRenderClient->ReleaseBuffer( framesAvailable, 0 );
    if ( FAILED( hr ) )
      return;
  }
}



