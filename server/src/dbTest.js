const { testConnection } = require('./db');

async function main() {
    try {
        const result = await testConnection();

        console.log('PostgreSQL connected successfully!');
        console.log('Database time:', result.now);
    } catch (error) {
        console.error('PostgreSQL connection failed:', error.message);
    }
}

main();