# pip install adafruit-io
# pip install opencv-python

from ultralytics import YOLO
import cvzone
import cv2
import math
import serial
import time
import threading
from Adafruit_IO import MQTTClient

AIO_USERNAME = "B3078"
AIO_KEY = "aio_WDfU46KXfH3QGZeZVgw14szcNtqH"
AIO_FEED = "info"

count = 0
ser = serial.Serial('COM3', 115200)

def connected(client):
    print("Connected to Adafruit IO!")
    client.subscribe(AIO_FEED)

def disconnected(client):
    print("Disconnected from Adafruit IO! Reconnecting...")
    time.sleep(5)
    client.connect()

def message(client, feed_id, payload):
    print(f"Received data from {feed_id}: {payload}")

client = MQTTClient(AIO_USERNAME, AIO_KEY)
client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message
client.connect()
client.loop_background()

def update_to_adafruit(value):
    try:
        print(f"Sending to Adafruit: {value}")
        client.publish(AIO_FEED, value)
    except Exception as e:
        print("Error publishing data:", e)

model = YOLO('best1.pt')
classnames = ['Buffalo', 'Elephant', 'Rhino', 'Zebra', "Cheetah", "Fox", "Jaguar", "Tiger", "Lion", "Panda"]

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    frame = cv2.resize(frame, (640, 480))
    result = model(frame, stream=True)

    detected_animal = None  # Reset for each frame

    for info in result:
        boxes = info.boxes
        for box in boxes:
            confidence = math.ceil(box.conf[0] * 100)
            Class = int(box.cls[0])

            if confidence > 50:
                x1, y1, x2, y2 = map(int, box.xyxy[0])
                cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 255), 5)
                label = f'{classnames[Class]} {confidence}%'
                cvzone.putTextRect(frame, label, [x1 + 8, y1 + 100], scale=1.5, thickness=2)

                detected_animal = classnames[Class]
                # print(f"Detected: {detected_animal}")


                if detected_animal == "Buffalo":
                    ser.write(b'A')
                elif detected_animal == "Elephant":
                    ser.write(b'B')
                if detected_animal == "Rhino":
                    ser.write(b'C')
                elif detected_animal == "Zebra":
                    ser.write(b'D')
                if detected_animal == "Cheetah":
                    ser.write(b'E')
                elif detected_animal == "Fox":
                    ser.write(b'F')
                if detected_animal == "Jaguar":
                    ser.write(b'G')
                elif detected_animal == "Tiger":
                    ser.write(b'H')
                if detected_animal == "Lion":
                    ser.write(b'I')
                elif detected_animal == "Panda":
                    ser.write(b'J')
                else:
                    ser.write(b'U')  # U = Unknown or Other

                # Send to Adafruit IO
                update_to_adafruit(detected_animal)

    cv2.imshow('frame', frame)

    if cv2.waitKey(1) == 27:  # ESC to exit
        break

cap.release()
cv2.destroyAllWindows()

