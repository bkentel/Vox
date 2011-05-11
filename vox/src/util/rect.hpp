#pragma once
#ifndef VOX_UTIL_RECT_HPP
#define VOX_UTIL_RECT_HPP

namespace vox {
    namespace util {

    template <typename T>
    class Rectangle {
    public:
        Rectangle(T left, T top, T right, T bottom)
            : x1_(left), y1_(top), x2_(right), y2_(bottom)
        {
            normalize_();
        }

        T left()   const { return x1_; }
        T right()  const { return x2_; }
        T bottom() const { return y2_; }
        T top()    const { return y1_; }
        
        T width()  const { return right() - left(); }
        T height() const { return bottom() - top(); }
        
        T area()   const { return width() * height(); }

        void resize(T dw, T dh) {
            x1_ += dw; x2_ += dw;
            y1_ += dh; y2_ += dh;

            normalize_();
        }

        void setSize(T w, T h) {
            x2_ = x1_ + w;
            y2_ = y1_ + h;
        }
    private:
        void normalize_() {
            if (left() > right()) std::swap(x1_, x2_);
            if (top() > bottom()) std::swap(y1_, y2_);
        }

        T x1_, y1_, x2_, y2_;
    };

    } //namespace util
} //namespace vox

#endif //VOX_UTIL_RECT_HPP
