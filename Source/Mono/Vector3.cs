using System;

namespace GIGA {
    public class Vector3 {
        public double x;
        public double y;
        public double z;
        
        public Vector3() {
            this.x = 0;
            this.y = 0;
            this.z = 0;
        }
        
        public Vector3(double x) {
            this.x = x;
            this.y = x;
            this.z = x;
        }
        
        public Vector3(double x, double y, double z) {
            this.x = x;
            this.y = y;
            this.z = z;
        }
        
        public static Vector3 operator +(Vector3 a, Vector3 b) {
            return new Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }
        
        public static Vector3 operator -(Vector3 a, Vector3 b) {
            return new Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }
        
        public static Vector3 operator *(Vector3 a, Vector3 b) {
            return new Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
        }
        
        public static Vector3 operator *(Vector3 a, double b) {
            return new Vector3(a.x * b, a.y * b, a.z * b);
        }
        
        public static Vector3 operator *(double b, Vector3 a) {
            return new Vector3(a.x * b, a.y * b, a.z * b);
        }
        
        public static Vector3 operator /(Vector3 a, Vector3 b) {
            return new Vector3(a.x / b.x, a.y / b.y, a.z / b.z);
        }
        
        public static Vector3 operator /(Vector3 a, double b) {
            return new Vector3(a.x / b, a.y / b, a.z / b);
        }
        
        public static Vector3 operator /(double b, Vector3 a) {
            return new Vector3(a.x / b, a.y / b, a.z / b);
        }
    }
}

