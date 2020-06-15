using System;

namespace GIGA {
    public class Vector4 {
        public double x;
        public double y;
        public double z;
        public double w;
        
        public Vector4() {
            this.x = 0;
            this.y = 0;
            this.z = 0;
            this.w = 0;
        }
        
        public Vector4(double x) {
            this.x = x;
            this.y = x;
            this.z = x;
            this.w = x;
        }
        
        public Vector4(double x, double y, double z, double w) {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }
        
        public static Vector4 operator +(Vector4 a, Vector4 b) {
            return new Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }
        
        public static Vector4 operator -(Vector4 a, Vector4 b) {
            return new Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }
        
        public static Vector4 operator *(Vector4 a, Vector4 b) {
            return new Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
        }
        
        public static Vector4 operator *(Vector4 a, double b) {
            return new Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
        }
        
        public static Vector4 operator *(double b, Vector4 a) {
            return new Vector4(a.x * b, a.y * b, a.z * b, a.w * b);
        }
        
        public static Vector4 operator /(Vector4 a, Vector4 b) {
            return new Vector4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
        }
        
        public static Vector4 operator /(Vector4 a, double b) {
            return new Vector4(a.x / b, a.y / b, a.z / b, a.w / b);
        }
        
        public static Vector4 operator /(double b, Vector4 a) {
            return new Vector4(a.x / b, a.y / b, a.z / b, a.w / b);
        }
    }
}

