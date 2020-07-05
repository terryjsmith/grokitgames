using System;
using System.Runtime.CompilerServices;

namespace GIGA {
	public partial class GigaObject {
        protected internal IntPtr ptr;
        
        public GigaObject() {
            if(ptr == IntPtr.Zero)
                ptr = internal_GigaObject_Ctor(this);
        }
        
        public virtual void Serialize(DataRecord record) { }
        public virtual void Deserialize(DataRecord record) { }
 
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static IntPtr internal_GigaObject_Ctor(GigaObject obj);
	}
}
