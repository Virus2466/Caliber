#pragma once

#include <cstdint>
#include<filesystem>
#include<glad/glad.h>

namespace Caliber{

class Texture{
    public:
        explicit Texture(const std::filesystem::path& path);
        ~Texture();

        // Non copyable
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& rhs) noexcept;
        Texture& operator=(Texture&& rhs) noexcept;

        // slot = 0 , 1 , 2..... (GL_TEXTURE0 , 1 ...)
        void bind(uint32_t slot = 0) const;
        void unbind() const;

        [[nodiscard]] uint32_t getID() const {return m_id;}
        [[nodiscard]] int getWidth() const {return m_width;}
        [[nodiscard]] int getHeight() const {return m_height;}
        [[nodiscard]] bool isLoaded() const {return m_loaded;}

    private:
        uint32_t m_id{0};
        int m_width{0};
        int m_height{0};
        bool m_loaded{false};


};

} // namespace caliber
