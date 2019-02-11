import QtQuick 2.2
import QtSensors 5.9

OrientationSensor{
    property Item target
    property bool enableTopUp: true
    property bool enableTopDown: true
    property bool enableLeftUp: true
    property bool enableRightUp: true
    active: true

    function setLandscape(){
        enableTopUp=false
        enableTopDown=false
        enableLeftUp=true
        enableRightUp=true
        if(target.rotation!=90 && target.rotation!=270)
            target.rotation=90
    }
    function setPortrait(){
        enableTopUp=true
        enableTopDown=true
        enableLeftUp=false
        enableRightUp=false
        if(target.rotation!=0 && target.rotation!=180)
            target.rotation=0
    }

    onReadingChanged: {
        if(enableTopUp
                && reading.orientation==OrientationReading.TopUp)
            target.rotation = 0
        else if(enableTopDown
                && reading.orientation==OrientationReading.TopDown)
            target.rotation = 180
        else if(enableLeftUp
                && reading.orientation==OrientationReading.LeftUp)
            target.rotation = 270
        else if(enableRightUp
                && reading.orientation==OrientationReading.RightUp)
            target.rotation = 90
    }
}
