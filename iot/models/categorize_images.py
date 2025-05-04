import os
import shutil
import pandas as pd

train_df = pd.read_csv("train.csv")

dataset_folder = "dataset_raw"
output_folder = "dataset"

print("CSV Columns:", train_df.columns)

categories = train_df["label"].unique()  

for category in categories:
    os.makedirs(os.path.join(output_folder, category), exist_ok=True)

for index, row in train_df.iterrows():
    filename = row["image_name"]
    label = row["label"]  

    file_path = os.path.join(dataset_folder, filename)
    dest_path = os.path.join(output_folder, label, filename)

    if os.path.exists(file_path):
        shutil.move(file_path, dest_path)
        print(f"Moved {filename} → {label}")
    else:
        print(f"File not found: {filename}")

print(" Dataset categorization completed!")
