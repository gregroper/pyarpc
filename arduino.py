from json_rpc import rpc_request


@rpc_request
def pinMode(pin, mode): pass


@rpc_request
def digitalWrite(pin, value): pass


@rpc_request
def digitalRead(pin): pass


@rpc_request
def analogReference(pin): pass


@rpc_request
def analogRead(pin): pass


@rpc_request
def analogWrite(pin, value): pass


@rpc_request
def tone(pin, frequency, duration): pass


@rpc_request
def noTone(pin): pass


@rpc_request
def shiftOut(dataPin, clockPin, bitOrder, value): pass


@rpc_request
def shiftIn(dataPin, clockPin, bitOrder): pass


@rpc_request
def pulseIn(pin, value, timeout): pass

