#pragma once

class ImageBS93;
class ImageBIOS;
class ImageCart;

class InputFile
{
public:
  enum class FileType
  {
    UNKNOWN,
    BIOS,
    BS93,
    CART,
  };

  InputFile( std::filesystem::path const& path );

  bool valid() const;
  FileType getType() const;

  std::shared_ptr<ImageBS93 const> getBS93() const;
  std::shared_ptr<ImageBIOS const> getBIOS() const;
  std::shared_ptr<ImageCart const> getCart() const;

private:
  FileType mType;
  std::shared_ptr<ImageBS93 const> mBS93;
  std::shared_ptr<ImageBIOS const> mBIOS;
  std::shared_ptr<ImageCart const> mCart;
};
