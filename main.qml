import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import zydd 1.0

Window {
    id: root
    visible: true
    width: 620
    height: 620
    title: qsTr("Fluid")

    Row {
        FluidSim {
            id: sim
            width: Math.min(root.width,root.height)
            height: Math.min(root.width,root.height)
            g: 0.07
            k: 4
            v: 0.05
            dt: 0.05
            factor: 10
        }

//        Column {
//            RowLayout {
//                Slider { id: g; from: -2; to: 2; stepSize: 0.01; value: 0.4 }
//                Label { text: 'g ' + g.value }
//            }

//            RowLayout {
//                Slider { id: k; from: 0; to: 20; stepSize: 0.01; value: 5 }
//                Label { text: 'k ' + k.value }
//            }

//            RowLayout {
//                Slider { id: v; from: 0; to: 2; stepSize: 0.001; value: 0.05 }
//                Label { text: 'v ' + v.value }
//            }

//            RowLayout {
//                Slider { id: dt; from: 0; to: 1; stepSize: 0.001; value: 0.05 }
//                Label { text: 'dt ' + dt.value }
//            }

//            RowLayout {
//                Slider { id: factor; from: 1; to: 100; stepSize: 1; value: 1 }
//                Label { text: 'factor' + factor.value }
//            }
//        }
    }
}
