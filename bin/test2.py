import cv2
import mediapipe as mp
import numpy as np

mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
mp_face_mesh = mp.solutions.face_mesh

# Landmark indices
LEFT_IRIS = [474, 475, 476, 477]
RIGHT_IRIS = [469, 470, 471, 472]
LEFT_EYE_H = [33, 133]
RIGHT_EYE_H = [362, 263]
LEFT_EYE_V = [159, 145]
RIGHT_EYE_V = [386, 374]

def get_iris_pos(iris_landmarks, eye_h, eye_v):
    iris_x = np.mean([lm.x for lm in iris_landmarks])
    iris_y = np.mean([lm.y for lm in iris_landmarks])
    x_ratio = (iris_x - eye_h[0].x) / (eye_h[1].x - eye_h[0].x)
    y_ratio = (iris_y - eye_v[0].y) / (eye_v[1].y - eye_v[0].y)
    return x_ratio, y_ratio

def classify_gaze(x_avg, y_avg):
    # Clamp values and map to 0, 1, 2
    x_grid = 0 if x_avg < 0.35 else 2 if x_avg > 0.65 else 1
    y_grid = 0 if y_avg < 0.35 else 2 if y_avg > 0.65 else 1
    return x_grid, y_grid

def draw_gaze_grid(image, x_index, y_index, xy_avg):
    h, w, _ = image.shape
    grid_size = 300
    top_left = (w // 2 - grid_size // 2, h // 2 - grid_size // 2)

    cell_w = grid_size // 3
    cell_h = grid_size // 3

    # Draw grid
    for i in range(3):
        for j in range(3):
            cell_x = top_left[0] + j * cell_w
            cell_y = top_left[1] + i * cell_h
            color = (0, 255, 0) if (j == x_index and i == y_index) else (200, 200, 200)
            thickness = 3 if (j == x_index and i == y_index) else 1
            cv2.rectangle(image, (cell_x, cell_y), (cell_x + cell_w, cell_y + cell_h), color, thickness)

    # Draw text
    text = f"Gaze Grid: ({x_index}, {y_index})"
    ratio_text = f"Est. XY Ratios: ({xy_avg[0]:.2f}, {xy_avg[1]:.2f})"
    cv2.putText(image, text, (top_left[0], top_left[1] - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (255, 255, 255), 2)
    cv2.putText(image, ratio_text, (top_left[0], top_left[1] + grid_size + 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)

drawing_spec = mp_drawing.DrawingSpec(thickness=1, circle_radius=1)
cap = cv2.VideoCapture(0)

with mp_face_mesh.FaceMesh(
    max_num_faces=1,
    refine_landmarks=True,
    min_detection_confidence=0.5,
    min_tracking_confidence=0.5) as face_mesh:

    while cap.isOpened():
        success, image = cap.read()
        if not success:
            print("Ignoring empty camera frame.")
            continue

        image.flags.writeable = False
        image_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        results = face_mesh.process(image_rgb)

        image.flags.writeable = True
        image = cv2.cvtColor(image_rgb, cv2.COLOR_RGB2BGR)

        if results.multi_face_landmarks:
            for face_landmarks in results.multi_face_landmarks:
                landmarks = face_landmarks.landmark

                # Left eye
                left_h = [landmarks[i] for i in LEFT_EYE_H]
                left_v = [landmarks[i] for i in LEFT_EYE_V]
                left_iris = [landmarks[i] for i in LEFT_IRIS]

                # Right eye
                right_h = [landmarks[i] for i in RIGHT_EYE_H]
                right_v = [landmarks[i] for i in RIGHT_EYE_V]
                right_iris = [landmarks[i] for i in RIGHT_IRIS]

                # Get iris positions
                x_left, y_left = get_iris_pos(left_iris, left_h, left_v)
                x_right, y_right = get_iris_pos(right_iris, right_h, right_v)

                # Average both eyes
                x_avg = (x_left + x_right) / 2
                y_avg = (y_left + y_right) / 2

                x_grid, y_grid = classify_gaze(x_avg, y_avg)

                draw_gaze_grid(image, x_grid, y_grid, (x_avg, y_avg))

        cv2.imshow('Gaze Grid Overlay', image)
        if cv2.waitKey(5) & 0xFF == 27:
            break

cap.release()
cv2.destroyAllWindows()
