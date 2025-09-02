// Author: Corey Ferguson
// Date:   2025 September 1  
// File    greeting.service.js
//

const { GreetingService } = require('./service/greeting.service'),
    { OpenAPIBackend } = require('openapi-backend'),
    cors = require("cors"),
    express = require("express"),
    path = require("path");

// Non-Member Fields

const PORT = 8080;

// Non-Member Functions

const main = () => {
    const api = new OpenAPIBackend({
        definition: path.join(__dirname, '/api-doc.yaml')
    })

    api.init();

    const handleError = (err, req, res) => {
        err.status ||= 500;

        res.status(err.status).send({
            message: err.message ?? "Unknown Error", status: err.status
        });  
    };
        greetingService = new GreetingService();

    api.register({
        validationFail: (c, req, res) => handleError({ message: c.validation.errors[0].message, status: 400 }, req, res),
        greeting: (c, req, res) => res.json(greetingService.createGreeting(req.body)),
        notFound: (c, req, res) => res.status(404).json(`Cannot ${req.method} ${req.url}`)
    })

    const app = express();

    app.use(cors({
        origin: "*"
    }));

    app.use(express.json());
    app.use((req, res) => api.handleRequest(req, req, res));

    app.listen(PORT, () => console.log(`Server listening on port ${PORT}...`));
};

// Entry point

main();