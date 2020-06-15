using System;

namespace GIGA {
	public class GameObject : Entity {
        public GameObject() {
            this.entityID = World.GetNextEntityID();
        }
 
        public virtual void Update(double delta) {
        
        }
        
        public virtual void Start() {
        
        }
	}
}
