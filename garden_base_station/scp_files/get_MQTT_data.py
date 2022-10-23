import paho.mqtt.client as mqtt  # to decode MQTT broker messages
import json
import os  # to help with logging and creating folder directories
import requests
import random

MQTT_ADDRESS = '192.168.1.65'
MQTT_USER = 'username'
MQTT_PASSWORD = 'test123'
MQTT_TOPIC = 'home/+/+'


def score_calculate(euid, temp_c, uv, humid, moist):
    """ Caluclates the score for the plant sensor """
    score = 0
    score_factor_counter = [0, 0, 0, 0]  # lux, temp, moisture, humidity
    sensor_array = [uv, temp_c, moist, humid]
    # 1 - Get Each Plant Ranges
    posting_array = {"euid": int(euid)}
    plants = requests.post(f"http://{MQTT_ADDRESS}/select_euid_plants.php", data=dict(posting_array))
    plant_array = json.loads(plants.text)
    # 2 - Loop through each plant data and check how many valid data points
    for attribute, plant in plant_array.items():
        plant_post = {"ID": int(plant)}
        plant_data = requests.post(f"http://{MQTT_ADDRESS}/get_plant_data_from_plant_table.php", data=dict(plant_post))
        plant_data = json.loads(plant_data.text)
        counter = 0
        for attr, data in plant_data.items():
            min_d = 0
            max_d = 0
            if counter % 2 != 0:
                min_d = int(data)
                if sensor_array[int(counter / 2)] < min_d: # or sensor_array[int(counter / 2)] > max_d:
                    score_factor_counter[int(counter / 2) - 1] += 1
            else:
                max_d = int(data)
            counter += 1

    # 3 - Get last record
    post = {"euid": int(euid)}
    prev_out = requests.post(f"http://{MQTT_ADDRESS}/get_score.php", data=dict(post))
    prev_out = json.loads(prev_out.text)

    # 4 - Correct the score and return
    if not sum(score_factor_counter):
        score = int(prev_out["score"]) + 1
        notification = 0
        notification_ctr = 0
    else:
        # set double moisture score
        score_factor_counter[2] = score_factor_counter[2] * 2
        score = int(prev_out["score"]) - sum(score_factor_counter)
        notification_ctr = int(prev_out["not_ctr"]) + 1
        if notification_ctr > 3:
            notification = 1
        else:
            notification = 0
    if score <= 0:
        score = 0
    elif score >= 100:
        score = 100
    return score, notification, notification_ctr


def directory_checker(filepath):
    """ Checks the filepath to see all folders in filepath exist. If not then create"""
    old_cwd = os.getcwd()
    os.chdir('/')
    path = ""
    for value in filepath.split("/"):
        path += value
        if not os.path.isdir(path):
            os.mkdir(path)
        path += "/"
    os.chdir(old_cwd)


def split_message(msg, charSplit):
    """ Returns an array of strings from msg split by charSplit"""
    output = str(msg).split(charSplit)
    return output[1:]


def log_message(msg, location):
    """ Logs the message upon receival"""
    msgPartition = split_message(msg, '/')
    score, notification, notification_count = score_calculate(int(msgPartition[0]), float(msgPartition[1]),
                                                              float(msgPartition[3]), float(msgPartition[4][:-1]),
                                                              float(msgPartition[2]))
    posting_array = {"score": score, "temp_c": float(msgPartition[1]),
                     "moisture": float(msgPartition[2]), "uv": float(msgPartition[3]),
                     "euid": int(msgPartition[0]), "humid": float(msgPartition[4][:-1]), "not": notification,
                     "not_cnt": notification_count}
    headers = {'content-type': 'application/json'}
    x = requests.post(f"http://{MQTT_ADDRESS}/db_create.php", data=dict(posting_array))
    print(x.text)


def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)


def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    print(msg.topic + ' ' + str(msg.payload))
    log_message(msg.payload, msg.topic)


def main():
    mqtt_client = mqtt.Client()
    mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()


if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()
