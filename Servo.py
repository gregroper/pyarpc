

class Servo(object):
    def __init__(self):
        self.id = servo_create()
        
    def __del__(self)
        servo_destroy(self.id)

    def attach(self, pin)
        servo_attach(self.id, pin)
        
    def read(self)
        return servo_read(self.id)
        
    def write(self, value)
        servo_write(self.id, value)
        
    def writeMicroseconds(self, value)
        servo_write_microseconds(self.id, value)
        
    def attached(self):
        return servo_attached(self.id)
    
    def detatch(self):
        servo_detatch(self.id)
  

@rpc_request
def servo_detach() {
  pass

    
@rpc_request
def servo_create()
    pass
            
            
@rpc_request
def servo_destroy(id):
    pass
    

@rpc_request    
def servo_attach() {
  pass
  

@rpc_request
def servo_write() {
  pass


@rpc_request
def servo_write_microseconds() {
  pass


@rpc_request
def servo_read() {
  pass


@rpc_request
def servo_attached() {
  pass
  

@rpc_request
def servo_detach() {
  pass

