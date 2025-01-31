let scenes = [];
let mSerial;
let readyToReceive = false;
let connectButton; // Connect Arduino button (global variable)

// Video & image files
let media = [
  { img: "Still1Test.jpg", video: "Scene1Test.mp4" },
  { img: "Still2Test.jpg", video: "Scene2Test.mp4" },
  { img: "Still3Test.jpg", video: "Scene3Test.mp4" },
  { img: "Still4Test.jpg", video: "Scene4Test.mp4" }
];

function preload() {
  for (let i = 0; i < media.length; i++) {
    let vid = createVideo(media[i].video);
    vid.hide(); // Hide HTML video
    vid.elt.muted = true; // Ensure no sound to avoid browser restrictions
    scenes.push({
      img: loadImage(media[i].img),
      video: vid,
      playing: false
    });
  }
}

function setup() {
  createCanvas(windowWidth, windowHeight);
  mSerial = createSerial();

  // Create Arduino connection button
  connectButton = createButton("Connect Arduino");
  connectButton.position(20, 20);
  connectButton.mousePressed(connectToSerial);

  console.log("Waiting for serial connection...");
}

function draw() {
  background(0);
  let sceneWidth = windowWidth / 4;
  let sceneHeight = windowHeight;

  for (let i = 0; i < scenes.length; i++) {
    let x = i * sceneWidth;
    if (scenes[i].playing) {
      image(scenes[i].video, x, 0, sceneWidth, sceneHeight);
    } else {
      image(scenes[i].img, x, 0, sceneWidth, sceneHeight);
    }
  }

  // Listen for Arduino serial data
  if (mSerial.opened() && readyToReceive) {
    readyToReceive = false;
    mSerial.clear();
    mSerial.write(0xab); // Send request to Arduino
  }

  if (mSerial.availableBytes() > 0) {
    receiveSerial();
  }
}

function playScene(index) {
  let scene = scenes[index];

  if (!scene.playing) {
    scene.playing = true;
    scene.video.play();

    scene.video.onended(() => {
      scene.playing = false;
      scene.video.hide();
    });
  }
}

function receiveSerial() {
  let line = mSerial.readUntil("\n");
  trim(line);
  if (!line) return;

  console.log("Received data: ", line);

  if (line.charAt(0) != "{") {
    console.log("Data parsing error: ", line);
    readyToReceive = true;
    return;
  }

  let json;
  try {
    json = JSON.parse(line);
  } catch (e) {
    console.error("JSON parsing failed: ", e);
    readyToReceive = true;
    return;
  }

  let data = json.data;
  console.log("Parsed data: ", data);

  // Parse Arduino button states and trigger video
  for (let i = 0; i < 4; i++) {
    if (data["button" + (i + 1)]) {
      console.log("Playing video: Scene", i + 1);
      playScene(i);
    }
  }

  readyToReceive = true;
}

function connectToSerial() {
  if (!mSerial.opened()) {
    mSerial.open(9600);
    readyToReceive = true;
    console.log("Serial connected");

    // **Completely remove the button**
    connectButton.remove();
  }
}

function windowResized() {
  resizeCanvas(windowWidth, windowHeight);
}
