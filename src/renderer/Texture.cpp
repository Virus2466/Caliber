#include "renderer/Texture.h"

#include <cstdint>
#include<iostream>
#include<utility>


#include <stb_image.h>

namespace Caliber{

Texture::Texture(const std::filesystem::path& path){
    glGenTextures(1,&m_id);
    glBindTexture(GL_TEXTURE_2D , m_id);

    // texture wrap
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT);

    // texture filtering
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);

    // load image(texture file)
    int channels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t* data = stbi_load(path.string().c_str(), &m_width, &m_height, &channels, 0);
    
    if(data){
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D , 0 , format , m_width , m_height , 0 , format , GL_UNSIGNED_BYTE , data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "[Texture] Loaded" << path << "(" << m_width << "x" << m_height << ")\n";
    } else{
        std::cerr << "[Texture] Failed to load : " << path << "\n";
    }

    stbi_image_free(data);

}

Texture::~Texture(){
    if(m_id != 0){
        glad_glDeleteTextures(1,&m_id);
    }
}

Texture::Texture(Texture&& rhs) noexcept
    : m_id(std::exchange(rhs.m_id , 0)),
      m_width(std::exchange(rhs.m_width , 0)),
      m_height(std::exchange(rhs.m_height, 0)){
}

Texture& Texture::operator=(Texture&& rhs) noexcept{
    if(this != &rhs){
        if(m_id != 0) glad_glDeleteTextures(1,&m_id);
        m_id = std::exchange(rhs.m_id , 0);
        m_width = std::exchange(rhs.m_width , 0);
        m_height = std::exchange(rhs.m_height , 0);
    }

    return *this;
}

void Texture::bind(uint32_t slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D , m_id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D , 0);
}


} // namespace caliber

