import json
import serial

_id = 0
_rpc_call_spec = {}
_rpc_calls = []
_sock = None


def open_port(portname, baudrate, timeout):
    global _sock  # Don't create a new local variable
    _sock = serial.Serial(portname, baudrate, timeout=timeout)


def close_port():
    _sock.close()


def make_noti(method, params=None):
    """
    Create notification - expects no response and therefore doesn't append a transaction ID
    :param method:
    :param params:
    :return:
    """
    noti = {"jsonrpc": "2.0", "method": method}
    if params is not None and len(params) > 0:
        noti["params"] = params
    return noti


def make_req(method, params=None):
    """
    Create request - expects a response, so uses a transaction ID
    :param method:
    :param params:
    :return:
    """
    req = make_noti(method, params)
    global _id
    req["id"] = _id
    _id += 1
    return req


def _send(_sock, req):
    """
    Send a request/notification to the socket
    :param req:
    :return:
    """
    json_req = json.dumps(req)
    _sock.write(json_req + '\n')


def _recv_result(_sock):
    """
    Receive a repsonse from the socket
    :return:
    """
    try:
        reply = _sock.readline()
    except Exception as e:
        return {'result': 'timeout-error'}

    try:
        json_reply = json.loads(reply)
    except Exception as e:
        print e, reply
        return {'result': 'error'}
    return json_reply


def _recv_flush(_sock):
    tmp = _sock.read(_sock.inWaiting())


def _walk_json_attributes(tree, attributes):
    for key, item in tree.iteritems():
        if isinstance(item, dict):
            _walk_json_attributes(item, attributes)
        else:
            attributes.append((key + '[' + str(len(item)) + ']' if isinstance(item, list) else key))


def _log_rpc_call(call, params_in, params_out):
    global _rpc_calls

    if call not in _rpc_call_spec:
        p_in = []
        p_out = []

        if params_in is None:
            p_in = ['none']
        else:
            p_in = ['?'] * len(params_in)

        if params_out is None:
            p_out = ['none']
        elif isinstance(params_out, dict):
            _walk_json_attributes(params_out, p_out)
        else:
            p_out = ['success_code']

        _rpc_call_spec[call] = {'in': p_in, 'out': p_out}

    _rpc_calls.append(call)


def save_rpc_call_info():
    import csv

    csv_file = open('rpc_api.csv', 'w')
    csv_writer = csv.writer(csv_file, dialect='excel', lineterminator='\n')
    csv_writer.writerow(['RPC call name', 'input count', 'output count', 'input', 'output'])

    for key, item in _rpc_call_spec.iteritems():
        csv_writer.writerow([key, len(item['in']), len(item['out']), ' '.join(item['in']), ' '.join(item['out'])])

    csv_file = open('rpc_calls.csv', 'w')
    csv_writer = csv.writer(csv_file, dialect='excel', lineterminator='\n')
    csv_writer.writerow(['calls'])

    for item in _rpc_calls:
        csv_writer.writerow([item])


def rpc_request(method):
    """
    Create a request with optional parameters and return the response
    :param method:
    :return:
    """

    def tmp(*params):

        # TODO: could ignore first parameter by
        # if len(args) > 1:
        #    print args[1:]

        _recv_flush(_sock)

        #TODO: append namespace names in the rpc instead of on individual functions
        # name = '_'.join((method.func_globals['__name__'], method.__name__))

        req = make_req(method.__name__, params)
        _send(_sock, req)

        reply = _recv_result(_sock)

        #_log_rpc_call(method.__name__, params, reply['result'])
        #_sock.close()

        return reply['result']

    return tmp


def rpc_notify(method):
    """
    Send a notification
    :param method:
    :return: Nothing
    """
    def tmp(*params):
        req = make_noti(method.__name__, params)
        _sock = serial.Serial('COM18', 230400, timeout=10)
        _send(_sock, req)

    return tmp
