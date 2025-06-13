import os
import requests

FOLDER_PATH = "./famine"
POCKETBASE_URL = "http://127.0.0.1:8090"
COLLECTION = "posts"
TOPIC_ID = "8tf81lyz0wf95g7"

headers = {
}

for filename in os.listdir(FOLDER_PATH):
    if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.gif')):
        with open(os.path.join(FOLDER_PATH, filename), "rb") as f:
            files = {
                "image": (filename, f),
            }
            data = {
                "field": TOPIC_ID
            }
            r = requests.post(f"{POCKETBASE_URL}/api/collections/{COLLECTION}/records", headers=headers, files=files, data=data)
            print(f"{filename}: {r.status_code} - {r.text}")
