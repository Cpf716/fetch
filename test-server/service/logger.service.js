// Author:  Corey Ferguson
// Date:    2025 September 1  
// File:    logger.service.js
// Project: fetch
//
// Typedef

class LoggerService {
    // Non-Member Functions

    static error = (...args) => console.error(...args)

    static info = (...args) => console.info(...args)
}

module.exports = { LoggerService };