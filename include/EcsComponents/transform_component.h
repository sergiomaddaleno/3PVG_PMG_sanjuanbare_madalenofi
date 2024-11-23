#ifndef TRNS
#define TRNS

struct Vec3 {
    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3(const Vec3& other) : x(other.x), y(other.y), z(other.z) {}
};

struct TransformComponent{
    Vec3 position;
    Vec3 rotation;
    Vec3 scale;
    float angle;

    TransformComponent() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), angle(0.0f) {}

    TransformComponent(Vec3 position_, Vec3 rotation_, Vec3 scale_, float angle_)
        : position(position_), rotation(rotation_), scale(scale_), angle(angle_) {}

    TransformComponent(const TransformComponent& other)
        : position(other.position), rotation(other.rotation), scale(other.scale), angle(other.angle) {}

    TransformComponent& operator=(const TransformComponent& other) {
        if (this != &other) {
            position = other.position;
            rotation = other.rotation;
            scale = other.scale;
            angle = other.angle;
        }
        return *this;
    }
};

#endif // TRNS
