using System;

namespace GIGA {
    public class Quaternion {
        public double x;
        public double y;
        public double z;
        public double w;
        
        public Quaternion() {
            x = y = z = 0.0f;
            w = 1.0f;
        }
        
        public Quaternion(double x, double y, double z, double w) {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }
        
        public static Quaternion operator *(Quaternion left, Quaternion right) {
            return new Quaternion
            (
                left.w * right.x + left.x * right.w + left.y * right.z - left.z * right.y,
                left.w * right.y + left.y * right.w + left.z * right.x - left.x * right.z,
                left.w * right.z + left.z * right.w + left.x * right.y - left.y * right.x,
                left.w * right.w - left.x * right.x - left.y * right.y - left.z * right.z
            );
        }
        
        public static Quaternion operator +(Quaternion left, Quaternion right) {
            return new Quaternion(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
        }

        public static Quaternion operator -(Quaternion left, Quaternion right) {
            return new Quaternion(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
        }
        
        public static Quaternion operator *(Quaternion left, Vector3 right) {
            return new Quaternion
            (
                left.w * right.x + left.y * right.z - left.z * right.y,
                left.w * right.y + left.z * right.x - left.x * right.z,
                left.w * right.z + left.x * right.y - left.y * right.x,
                -left.x * right.x - left.y * right.y - left.z * right.z
            );
        }

        public static Quaternion operator *(Vector3 left, Quaternion right) {
            return new Quaternion
            (
                right.w * left.x + right.y * left.z - right.z * left.y,
                right.w * left.y + right.z * left.x - right.x * left.z,
                right.w * left.z + right.x * left.y - right.y * left.x,
                -right.x * left.x - right.y * left.y - right.z * left.z
            );
        }
        
        public static Quaternion operator *(Quaternion left, double right) {
            return new Quaternion(left.x * right, left.y * right, left.z * right, left.w * right);
        }

        public static Quaternion operator *(double left, Quaternion right) {
            return new Quaternion(right.x * left, right.y * left, right.z * left, right.w * left);
        }
    }
}
