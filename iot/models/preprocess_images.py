import os
import cv2
import numpy as np
import pickle
from sklearn.model_selection import train_test_split 

IMG_SIZE = (128, 128)
categories = ["small_animals", "large_animals", "nocturnal_animals"]
data, labels = [], []


for category in categories:
    path = os.path.join("dataset", category)
    label = categories.index(category)

    for img in os.listdir(path):
        img_array = cv2.imread(os.path.join(path, img))
        img_array = cv2.resize(img_array, IMG_SIZE)
        data.append(img_array)
        labels.append(label)

data = np.array(data) / 255.0  
labels = np.array(labels)

X_train, X_test, y_train, y_test = train_test_split(data, labels, test_size=0.2)

with open("preprocessed_data.pkl", "wb") as f:
    pickle.dump((X_train, X_test, y_train, y_test), f)

print(" Dataset preprocessing completed!")
