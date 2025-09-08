import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Spotify

Window {
    id: root
    visible: true
    SpotifyApi {
        id: api
        clientId: "d66dea877b004271813e5579438321df"
        scope: ["user-read-playback-state", "user-modify-playback-state", "playlist-read-private", "user-library-read"]
        Component.onCompleted: api.init()
    }
    ColumnLayout {
        anchors.centerIn: parent
        Text {
            text: api.authorized ? "Authorized" : "Not authorized"
        }
        Text {
            text: `User: ${JSON.stringify(api.currentUser, null, 2)}`
        }

        Button {

            text: "update current user"

            onClicked: api.updateCurrentUser()
        }
        Button {

            text: "authorize"

            onClicked: api.authorize()
        }
    }
}
