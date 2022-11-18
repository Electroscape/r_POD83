import cv2

try:
    camera = cv2.VideoCapture("rtsp://Terminal:POD83Cam@192.168.87.177:88/videoMain")
except:
    camera = cv2.VideoCapture("rtsp://rtsp.stream/pattern")

# camera = cv2.VideoCapture("rtsp://TeamEscape:*********@192.168.88.21:88/videoMain")
# camera = cv2.VideoCapture(1)


def gen_frames():
    '''
    for ip camera use - rtsp://username:password@ip_address:88/videoMain' 
    for local webcam use cv2.VideoCapture(0)
    '''

    while True:
        success, frame = camera.read()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result
