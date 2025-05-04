import cv2
import numpy as np
import joblib
from tensorflow.keras.models import load_model, Model

# Load models
cnn_model = load_model("cnn_animal_classifier.h5")
svm_model = joblib.load("svm_animal_classifier.pkl")

# Feature extractor (CNN without last layer)
feature_extractor = Model(inputs=cnn_model.input, outputs=cnn_model.layers[-2].output)

# Define categories
categories = ["small_animals", "large_animals", "nocturnal_animals"]

def predict_animal(image_path):
    img = cv2.imread(image_path)
    img = cv2.resize(img, (128, 128))
    img = np.expand_dims(img, axis=0)

    # Extract CNN features
    cnn_features = feature_extractor.predict(img)

    # Predict using SVM
    prediction = svm_model.predict(cnn_features)

    return categories[prediction[0]]

# Test with an image
print("Predicted Animal Type:", predict_animal("test_image.jpg"))
