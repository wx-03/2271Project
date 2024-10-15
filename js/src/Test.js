import { useState } from "react";
import { Joystick } from "react-joystick-component";

// Function to convert joystick positions to wheel speeds
function joystickToWheelSpeeds(x, y) {
  // Normalize the speeds to ensure they are within the maximum possible range
  const angle = Math.atan2(y, x) % (Math.PI / 2);
  var maxSpeed = Math.sqrt(2) * Math.sin(angle + Math.PI / 4);

  // Reverse x direction if moving backward
  if (y < 0) {
    x = -x;
    maxSpeed = 2 + maxSpeed;
  }

  // Calculate the wheel speeds
  let leftWheelSpeed = y + x;
  let rightWheelSpeed = y - x;

  // Avoid division by zero in normalization
  if (maxSpeed !== 0) {
    leftWheelSpeed /= maxSpeed;
    rightWheelSpeed /= maxSpeed;
  }

  return {
    leftWheelSpeed: leftWheelSpeed,
    rightWheelSpeed: rightWheelSpeed,
  };
}

// Function to map wheel speed to a 4-bit value (0-15)
function mapSpeedTo4Bit(speed) {
  return Math.round((speed + 1) * 7.5);
}

// Function to combine two 4-bit values into a single 8-bit value
function combine4BitValues(left4Bit, right4Bit) {
  return (left4Bit << 4) | right4Bit;
}



export function Test() {
  const [joystickPos, setJoystickPos] = useState({ x: 0, y: 0 });
  const [binarySpeed, setBinarySpeed] = useState("01110111");
  const [currTime, setCurrTime] = useState(Date.now());

  function callReq() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.114.249/data');
    request.send(JSON.stringify({data : binarySpeed}))
  }

  function callStop() {
    var request = new XMLHttpRequest();
    request.open('POST', 'http://192.168.114.249/data');
    request.send(JSON.stringify({data : "01110111"}))
  }

  const handleMove = (stick) => {
    setJoystickPos({ x: stick.x, y: stick.y });

    const { leftWheelSpeed, rightWheelSpeed } = joystickToWheelSpeeds(
      stick.x,
      stick.y
    );

    // Convert wheel speeds to 4-bit values
    const leftWheel4Bit = mapSpeedTo4Bit(leftWheelSpeed);
    const rightWheel4Bit = mapSpeedTo4Bit(rightWheelSpeed);
    const combined8BitValue = combine4BitValues(leftWheel4Bit, rightWheel4Bit);

    const binaryValue = combined8BitValue.toString(2).padStart(8, "0");
    setBinarySpeed(binaryValue);
    if (Date.now() - currTime > 1000) {
      setCurrTime(Date.now());
      callReq();
    } 
    console.log("Combined 8-bit Value (Binary):", binaryValue);
  };

  const handleStop = () => {
    setJoystickPos({ x: 0, y: 0 });
    setBinarySpeed("01110111");
    callStop();
  };

  return (
    <div>
      <Joystick pos={joystickPos} move={handleMove} stop={handleStop} />
      <p>Binary Speed: {binarySpeed}</p>
    </div>
  );
}
