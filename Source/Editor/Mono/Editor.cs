using System;
using System.Runtime.CompilerServices;

namespace GIGA
{
	public class Editor
    {
        public static void LoadLibrary()
        {
            // Define MeshComponent type
            Editor.SetFieldType("MeshComponent", "Mesh.mesh", "Mesh");
            Editor.SetFieldAction("MeshComponent", "Mesh.mesh", "SelectOne");
            Editor.SetFieldDefault("MeshComponent", "Mesh.mesh", "box.fbx");
        }
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetFieldType(string className, string field, string type);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetFieldAction(string className, string field, string action);
        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public extern static void SetFieldDefault(string className, string field, string value);
	}
}
