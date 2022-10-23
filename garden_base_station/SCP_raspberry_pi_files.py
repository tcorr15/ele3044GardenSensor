from paramiko import SSHClient
from paramiko import AutoAddPolicy
from paramiko import SFTPClient
from paramiko import Transport
import time
from scp import SCPClient
import os


"""
SCP Network information
"""
Host = "192.168.0.20"
Port = 22
Username = "pi"
Password = "raspberry"
commandArray = ['sudo apt update && sudo apt upgrade -y', 'sudo apt install -y mosquitto mosquitto-clients',
                'sudo systemctl enable mosquitto.service', 'sudo pip install paho-mqtt', 'sudo apt install apache2 -y',
                'cd /var/www/html', 'sudo apt install php -y', 'sudo rm index.html',
                'sudo apt install mariadb-server php-mysql -y', 'sudo service apache2 restart',
                'sudo mysql -e "create user admin@localhost identified by "your_password";grant all privileges on *.* to admin@localhost;FLUSH PRIVILEGES;exit;"',
                'sudo phpenmod mysqli', 'sudo service apache2 restart',
                'sudo ln -s /usr/share/phpmyadmin /var/www/html/phpmyadmin', 'sudo apt-get install openssl',
                'sudo mkdir -p /etc/ssl/mycerts',
                'sudo openssl req -new -x509 -days 365 -nodes -out /etc/ssl/mycerts/apache.pem -keyout /etc/ssl/mycerts/apache.key',
                'sudo chmod 600 /etc/ssl/mycerts/apache*', 'sudo a2ensite default-ssl', 'sudo a2ensite default-ssl']
# commandArray2 = ['cd /tmp', 'chmod a+x mysql_secure_installation_test.sql',
#                  'mysql -sfu root < "mysql_secure_installation.sql"']


class MySFTPClient(SFTPClient):
    def put_dir(self, source, target):
        """ Uploads the contents of the source directory to the target path. The
            target directory needs to exists. All subdirectories in source are
            created under target.
        """
        for item in os.listdir(source):
            if os.path.isfile(os.path.join(source, item)):
                self.put(os.path.join(source, item), '%s/%s' % (target, item))
            else:
                self.mkdir('%s/%s' % (target, item), ignore_existing=True)
                self.put_dir(os.path.join(source, item), '%s/%s' % (target, item))

    def mkdir(self, path, mode=511, ignore_existing=False):
        """ Augments mkdir by adding an option to not fail if the folder exists  """
        try:
            super(MySFTPClient, self).mkdir(path, mode)
        except IOError:
            if ignore_existing:
                pass
            else:
                raise


class SSH_SCP:
    def __init__(self, host, port, username, password):
        """
        Sets the parameters of the initialisation function to the objects private variables
        """
        self.host = host
        self.port = port
        self.username = username
        self.password = password
        self.ssh = SSHClient()  # sets a new SSHClient object from paramiko library
        self.ssh.set_missing_host_key_policy(AutoAddPolicy())
        self.ssh.connect(self.host, self.port, self.username, self.password)

    def transferSCPfile(self, file, remotePath):
        """
        Transfers a given file into the temp folder using SCP protocol
        """
        print(f"Transferring {file} via SCP")
        self.ssh.connect(self.host, self.port, self.username, self.password)
        scp = SCPClient(self.ssh.get_transport())
        filename = file.split("/")
        try:
            with SCPClient(self.ssh.get_transport()) as scp:
                scp.put(file, recursive=True, remote_path=remotePath)
            scp.close()
        except:
            self.runCommand(f"sudo chmod -R 777 {remotePath}")
            scp = SCPClient(self.ssh.get_transport())
            sftp = self.ssh.open_sftp()
            sftp.chdir("/tmp")
            sftp.put(file, os.path.join(remotePath, "tmp"))
            remoteSplit = remotePath.split("/")
            remotePathNew = "\\" + remoteSplit[0]
            for file in remoteSplit[1:]:
                remotePathNew = os.path.join(remotePathNew, file)
            sftp.rename(os.path.join(remotePath, "tmp"), remotePathNew)
            sftp.close()
            scp.close()
        self.ssh.close()

    def runCommand(self, command):
        """
        Runs a given command
        """
        self.ssh.connect(self.host, self.port, self.username, self.password)
        print(f'Running => {command}...\n')
        stdin, stdout, stderr = self.ssh.exec_command(command)
        time.sleep(1)
        for line in iter(stdout.readline, ""):
            print(line, end="")
        self.ssh.close()


"""
Run all commands
"""
print("\n\n----Running Installation (1/4)-----")
ssh_service = SSH_SCP(Host, Port, Username, Password)
print("Package Installation Complete")


"""
Transfer all files
"""
print("\n\n----Transferring all files (2/4)-----")
ssh_service.transferSCPfile((os.getcwd() + "/scp_files/get_MQTT_data.py"), "/home/pi")
ssh_service.transferSCPfile((os.getcwd() + "/scp_files/mosquitto.conf"), "/etc/mosquitto")
ssh_service.transferSCPfile((os.getcwd() + "/scp_files/pwfile"), "/etc/mosquitto")
ssh_service.transferSCPfile((os.getcwd() + "/scp_files/mysql_secure_installation_test.sql"), "/tmp")
ssh_service.transferSCPfile((os.getcwd() + "/scp_files/db_view.php"), "/var/www/html")
ssh_service.transferSCPfile((os.getcwd() + "/scp_files/db_create.php"), "/var/www/html")
print("File Transfer Complete")

"""
Run all commands
"""
print("\n\n----Running Installation (3/4)-----")
ssh_service = SSH_SCP(Host, Port, Username, Password)
print("Package Installation Complete")


"""
Reboot Raspberry Pi
"""
# print("\n\n----Rebooting Raspberry Pi (4/4)-----")
# ssh_service.runCommand("sudo reboot")
# print("Update Complete")
