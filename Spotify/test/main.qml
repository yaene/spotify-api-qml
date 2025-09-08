import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Spotify

Window {
    id: root
    visible: true
    ColumnLayout {
        anchors.centerIn: parent
        Text {
            text: SpotifyApi.authorized ? "Authorized" : "Not authorized"
        }
        Text {
            text: `User: ${JSON.stringify(SpotifyApi.currentUser, null, 2)}`
        }

        Button {

            text: "update current user"

            onClicked: SpotifyApi.updateCurrentUser()
        }
        Button {

            text: "authorize"

            onClicked: SpotifyApi.authorize("d66dea877b004271813e5579438321df", ["user-read-playback-state", "user-modify-playback-state", "playlist-read-private", "user-library-read"])
        }
    }
}
