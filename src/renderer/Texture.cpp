#include "renderer/Texture.h"

#include <cstdint>
#include<iostream>
#include<utility>


#include <stb_image.h>

namespace Caliber{

Texture::Texture(const std::filesystem::path& path, TextureColorSpace colorSpace){
    // Assimp supplies bottom-left UVs (including for glTF). Upload the bottom row first.
    stbi_set_flip_vertically_on_load_thread(true);
    stbi_uc* pixels = stbi_load(path.string().c_str(), &m_width, &m_height, nullptr, STBI_rgb_alpha);
    if (!pixels) {
        const char* reason = stbi_failure_reason();
        std::cerr << "[Texture] Failed to load: " << path << " (" << (reason ? reason : "unknown") << ")\n";
        return;
    }

    glGenTextures(1,&m_id);
    glBindTexture(GL_TEXTURE_2D , m_id);

    // texture wrap
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT);

    // texture filtering
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);

    const GLint internalFormat = colorSpace == TextureColorSpace::SRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    GLint unpackAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpackAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment);
    stbi_image_free(pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    m_loaded = true;
    std::cout << "[Texture] Loaded " << path << " (" << m_width << "x" << m_height << ")\n";

}

Texture::~Texture(){
    if(m_id != 0){
        glad_glDeleteTextures(1,&m_id);
    }
}

Texture::Texture(Texture&& rhs) noexcept
    : m_id(std::exchange(rhs.m_id , 0)),
      m_width(std::exchange(rhs.m_width , 0)),
      m_height(std::exchange(rhs.m_height, 0)),
      m_loaded(std::exchange(rhs.m_loaded, false)){
}

Texture& Texture::operator=(Texture&& rhs) noexcept{
    if(this != &rhs){
        if(m_id != 0) glad_glDeleteTextures(1,&m_id);
        m_id = std::exchange(rhs.m_id , 0);
        m_width = std::exchange(rhs.m_width , 0);
        m_height = std::exchange(rhs.m_height , 0);
        m_loaded = std::exchange(rhs.m_loaded, false);
    }

    return *this;
}

void Texture::bind(uint32_t slot) const {
    if(!m_loaded || m_id == 0){
        std::cerr << "[Texture] Tried to bind an unloaded texture on slot " << slot << "\n";
        return;
    }

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D , m_id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D , 0);
}


} // namespace caliber

