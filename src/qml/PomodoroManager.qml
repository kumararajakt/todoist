import QtQuick

QtObject {
    id: manager
    
    property int activeTaskId: -1
    property real currentProgress: 0.0
    property bool isRunning: false
    property bool isBreak: false
    property int timeRemaining: 0
    
    signal progressChanged(int taskId, real progress)
    signal timerStateChanged(int taskId, bool running, bool isBreak)
    
    function setActiveTask(taskId) {
        if (activeTaskId !== -1 && activeTaskId !== taskId) {
            // Stop previous timer
            stopTimer()
        }
        activeTaskId = taskId
    }
    
    function startTimer(taskId) {
        setActiveTask(taskId)
        isRunning = true
        timerStateChanged(taskId, true, isBreak)
    }
    
    function stopTimer() {
        isRunning = false
        currentProgress = 0.0
        if (activeTaskId !== -1) {
            timerStateChanged(activeTaskId, false, isBreak)
            progressChanged(activeTaskId, 0.0)
        }
        activeTaskId = -1
    }
    
    function updateProgress(progress) {
        currentProgress = progress
        if (activeTaskId !== -1) {
            progressChanged(activeTaskId, progress)
        }
    }
    
    function getProgressForTask(taskId) {
        return (taskId === activeTaskId) ? currentProgress : 0.0
    }
    
    function isTaskActive(taskId) {
        return taskId === activeTaskId && isRunning
    }
}