from tensorflow.keras.models import Model
from sklearn.svm import SVC
import pickle
import joblib
import numpy as np
from tensorflow.keras.models import load_model

# Load preprocessed data
with open("preprocessed_data.pkl", "rb") as f:
    X_train, X_test, y_train, y_test = pickle.load(f)

# Load CNN model and remove last layer
cnn_model = load_model("cnn_animal_classifier.h5")
feature_extractor = Model(inputs=cnn_model.input, outputs=cnn_model.layers[-2].output)

# Extract CNN features
X_train_features = feature_extractor.predict(X_train)
X_test_features = feature_extractor.predict(X_test)

# Train SVM on CNN features
svm_model = SVC(kernel="linear")
svm_model.fit(X_train_features, y_train)

# Save SVM model
joblib.dump(svm_model, "svm_animal_classifier.pkl")

print("✅ SVM Model Trained & Saved!")
