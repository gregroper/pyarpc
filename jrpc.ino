#include "json_rpc.h"
#include <Servo.h>

#define isnewline(x) x == '\n'

#define uart Serial

static char line_buffer[128];
static const int json_buffer_size = 512;
static char json_buffer[json_buffer_size];

char rx;
int line_buffer_index = 0;

int pin_mode(Argument *args, char *ret) { 
  int pin = args->pop_int();
  int mode = args->pop_int();
  pinMode(pin, mode);
  return success_no_data;
}

int digital_write(Argument *args, char *ret) {
  int pin = args->pop_int();
  int value = args->pop_int();
  digitalWrite(pin, value);
  return success_no_data;
}

int digital_read(Argument *args, char *ret) {
  int pin = args->pop_int();
  sprintf(ret, "%d", digitalRead(pin));
  return success;
}

int analog_reference(Argument *args, char *ret) 
{
  analogReference(args->pop_int());
  return success_no_data;
}

int analog_read (Argument *args, char *ret) 
{
  sprintf(ret, "%e", analogRead(args->pop_int()));
  return success;
}

int analog_write(Argument *args, char *ret) 
{
  analogWrite(args->pop_int(), args->pop_int());
  return success_no_data;
}

int tone_(Argument *args, char *ret) {
  int pin = args->pop_int();
  int freq = args->pop_int();
  int duration = args->pop_int();
  
  tone(pin, freq, duration);
return success_no_data;
}

int no_tone(Argument *args, char *ret) 
{
  noTone(args->pop_int());
  return success_no_data;
}

int shift_out (Argument *args, char *ret) 
{
  shiftOut(args->pop_int(), args->pop_int(), args->pop_int(), args->pop_int());
  return success_no_data;
}

int shift_in(Argument *args, char *ret) 
{
  sprintf(ret, "%d", shiftIn (args->pop_int(), args->pop_int(), args->pop_int()));
  return success;
}

int pulse_in (Argument *args, char *ret) 
{
  sprintf(ret, "%ul", pulseIn(args->pop_int(), args->pop_int(), args->pop_int()));
  return success;
}


int servo_create(Argument *args, char *ret) {
  Servo *servo = new Servo();
  sprintf(ret, "%d", (int)servo);
  return success;
}

int servo_destroy(Argument *args, char *ret) {
  delete reinterpret_cast<Servo *>(args->pop_int());
}

int servo_attach(Argument *args, char *ret) {
  Servo *servo = reinterpret_cast<Servo *>(args->pop_int());
  sprintf(ret, "%d", servo->attach(args->pop_int()));
  return success;
}

int servo_write(Argument *args, char *ret) {
  Servo *servo = reinterpret_cast<Servo *>(args->pop_int());
  servo->write(args->pop_int());
  return success_no_data;
}

int servo_write_microseconds(Argument *args, char *ret) {
  Servo *servo = reinterpret_cast<Servo *>(args->pop_int());
  servo->writeMicroseconds(args->pop_int());
  return success_no_data;
}

int servo_read(Argument *args, char *ret) {
  Servo *servo = reinterpret_cast<Servo *>(args->pop_int());
  sprintf(ret, "%d", servo->read());
  return success;
}

int servo_attached(Argument *args, char *ret) {
  Servo *servo = reinterpret_cast<Servo *>(args->pop_int());
  sprintf(ret, "%d", servo->attached());
  return success;
}

int servo_detach(Argument *args, char *ret) {
  Servo *servo = reinterpret_cast<Servo *>(args->pop_int());
  servo->detach();
  return success_no_data;
}

//TODO: Move to progmem
command_t api_command_table[] = {
  {"pinMode", pin_mode },
  {"digitalWrite", digital_write},
  {"digitalRead", digital_read},
  {"analogReference", analog_reference},
  {"analogRead", analog_read},
  {"analogWrite", analog_write},
  {"tone", tone_},
  {"noTone", no_tone},
  {"shiftOut", shift_out},
  {"shiftIn", shift_in},
  {"pulseIn", pulse_in},
  
  
  {"servo_attach", servo_attach},
  {"servo_write", servo_write},
  {"servo_writeMicroseconds", servo_write_microseconds},
  {"servo_read", servo_read},
  {"servo_attached", servo_attached},
  {"servo_detach", servo_detach},
};

int api_num_commands = sizeof(api_command_table) / sizeof(api_command_table[0]);
    
void setup()
{
    uart.begin(115200); //Will run this as fast as we reliably can
    uart.println("RPC");
}

void loop()
{
     if (uart.available() > 0)
     {
        rx = uart.read();

        if (isprint(rx) != 0)
        {
            // if maximum line length exceeded drop characters.
            if (line_buffer_index < API_MAX_LINE_LENGTH - 1)
            {
                line_buffer[line_buffer_index++] = rx;
                line_buffer[line_buffer_index] = '\0';
            }
        }
        else if (isnewline(rx))
        {
            return_t ret = json_rpc_handle_command(line_buffer, json_buffer);

            if ((ret == success) || (ret == success_no_data))
            {
                uart.print("{\"jsonrpc\":\"2.0\",\"result\":");
                uart.print(ret == success ? json_buffer : "null");
                uart.print(",\"id\":null}"); //TODO: id
                uart.write('\n');
            }
            else if(ret == success_no_response)
            {
            }
            else if (ret == fail)
            {
                uart.print("{\"jsonrpc\":\"2.0\",\"error\":");
                uart.print(json_buffer);
                uart.print(",\"id\":null}");
                uart.write('\n');
            }
            else if (ret == fail_no_method)
            {
                uart.print("{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32601,\"message\":\"Procedure not found\"},\"id\":null}");
                uart.write('\n');
            }
            line_buffer_index = 0;
            line_buffer[line_buffer_index] = '\0';
        }
     }   
}

