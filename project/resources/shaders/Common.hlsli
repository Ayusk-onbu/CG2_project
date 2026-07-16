#ifndef MY_COMMON_INCLUDED
#define MY_COMMON_INCLUDED

struct PerView
{
    float32_t4x4 viewProjection;
    float32_t4x4 billboardMatrix;
};

#endif// MY_COMMON_INCLUDED
// pragma onceでもいいらしいがかっこいいからこっち