using System;
using System.Runtime.CompilerServices;

namespace GIGA {
	public class GigaObject {
        protected internal IntPtr ptr;
        
        public GigaObject() {
            if(ptr == IntPtr.Zero)
                ptr = internal_GigaObject_Ctor(this);
        }
 
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static IntPtr internal_GigaObject_Ctor(GigaObject obj);
	}
}
