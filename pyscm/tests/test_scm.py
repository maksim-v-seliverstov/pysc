import os
import sys
import uuid
import socket
import unittest
from xmlrpc.client import ServerProxy


sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..'))

import pyscm

SERVER_PORT = 15742


# Client code


class SCMTest(unittest.TestCase):

    def test_skeleton(self):
        repeat_count = 2
        client = ServerProxy('http://%s:%s' % (socket.gethostbyname(socket.gethostname()), SERVER_PORT))
        for _ in range(repeat_count):
            service_name = 'pyscm_test1'
            script_path = os.path.abspath(__file__)
            args = [str(i) for i in range(10)]
            msg = str(uuid.uuid1())

            with self.assertRaises(ConnectionRefusedError):
                client.msg(msg)

            pyscm.create(
                service_name=service_name,
                cmd=[sys.executable, script_path] + args
            )
            try:
                pyscm.start(service_name)
                self.assertEqual(client.echo(msg), msg)
                self.assertEqual(client.get_args(), [str(arg) for arg in args])
                pyscm.stop(service_name)
            finally:
                pyscm.delete(service_name)

            with self.assertRaises(ConnectionRefusedError):
                    client.msg(msg)


# Server code


from xmlrpc.server import SimpleXMLRPCServer

from pyscm import event_stop


class TestServer:
    def __init__(self, args):
        self.args = args

    def echo(self, msg):
        return msg

    def get_args(self):
        return self.args


if __name__ == '__main__':
    timeout = 20
    server = SimpleXMLRPCServer(
        (socket.gethostbyname(socket.gethostname()), SERVER_PORT)
    )

    @event_stop
    def stop():
        server.server_close()

    server.register_instance(TestServer(sys.argv[1:]))
    server.serve_forever()
