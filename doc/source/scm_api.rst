Service API
===========

.. py:function:: pysc.create(service_name, cmd, [username=None[, password=None]])

   Create service.

   :param str service_name: The name of the service to install.
   :param cmd: The fully qualified path to the service binary file. The path can also include arguments for an auto-start service.
   :type cmd: list or str
   :param username: The name of the account under which the service should run. Default uses the LocalSystem account.
   :type username: str or None
      :platform: Windows
   :param password: The password to the account name specified by the *username* parameter.
   :type password: str or None
      :platform: Windows
   :raises WinError: if the GetLastError return non zero value.

.. py:function:: pysc.delete(service_name)

   Delete service.

   :param str service_name: The name of the service.

.. py:function:: pysc.start(service_name)

   Start service.

   :param str service_name: The name of the service.

.. py:function:: pysc.stop(service_name)

   Stop service.

   :param str service_name: The name of the service.

.. py:function:: pysc.event_stop(close_func)

   Specify a function for stop the process of a service.

   :param close_func: A function is necessary to stop the process.

.. py:function:: pysc.set_user(username[, password=None[, service_name=None]])

   Specify name and password of the account to run service.

   :platform: Windows
   :param str username: The name of the account under which the service should run. Default uses the LocalSystem account.
   :param password: The password to the account name specified by the *username* parameter.
   :type password: str or None
   :param service_name: The name of the service.
   :type password: str or None

.. note:: On Windows if you specify user name to run service, you should add the "Log on as a service" right to an account on your local computer.
          (**Local Security Policy** -> **Local Policies** ->
          **User Rights Assignment** -> **Log on as a service**).
