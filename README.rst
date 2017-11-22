Installation
-------------

::

   $ pip install pysc

Getting started
----------------

Example script to run as a service
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   from xmlrpc.server import SimpleXMLRPCServer

   from psc import event_stop


   class TestServer:

       def echo(self, msg):
           return msg


   if __name__ == '__main__':
       server = SimpleXMLRPCServer(('127.0.0.1', 9001))

       @event_stop
       def stop():
           server.server_close()

       server.register_instance(TestServer())
       server.serve_forever()



Create and start service
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import os
   import sys
   from xmlrpc.client import ServerProxy

   import pysc


   if __name__ == '__main__':
       service_name = 'test_xmlrpc_server'
       script_path = os.path.join(
           os.path.dirname(__file__), 'xmlrpc_server.py'
       )
       pysc.create(
           service_name=service_name,
          cmd=[sys.executable, script_path]
       )
       pysc.start(service_name)

       client = ServerProxy('http://127.0.0.1:9001')
       print(client.echo('test scm'))



Stop and delete service
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: python

   import os

   import pysc


   if __name__ == '__main__':
       service_name = 'test_xmlrpc_server'
       pysc.stop(service_name)
       pysc.delete(service_name)
