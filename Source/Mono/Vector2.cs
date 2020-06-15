using System;

namespace GIGA {
    public class Vector2 {
        public double x;
        public double y;
        
        public Vector2() {
            this.x = 0;
            this.y = 0;
        }
        
        public Vector2(double x) {
            this.x = x;
            this.y = x;
        }
        
        public Vector2(double x, double y) {
            this.x = x;
            this.y = y;
        }
        
        public static Vector2 operator +(Vector2 a, Vector2 b) {
            return new Vector2(a.x + b.x, a.y + b.y);
        }
        
        public static Vector2 operator -(Vector2 a, Vector2 b) {
            return new Vector2(a.x - b.x, a.y - b.y);
        }
        
        public static Vector2 operator *(Vector2 a, Vector2 b) {
            return new Vector2(a.x * b.x, a.y * b.y);
        }
        
        public static Vector2 operator *(Vector2 a, double b) {
            return new Vector2(a.x * b, a.y * b);
        }
        
        public static Vector2 operator *(double b, Vector2 a) {
            return new Vector2(a.x * b, a.y * b);
        }
        
        public static Vector2 operator /(Vector2 a, Vector2 b) {
            return new Vector2(a.x / b.x, a.y / b.y);
        }
        
        public static Vector2 operator /(Vector2 a, double b) {
            return new Vector2(a.x / b, a.y / b);
        }
    }
}
