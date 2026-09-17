const { Pool } = require('pg');

const pool = new Pool();

async function query(text, params) {
    const result = await pool.query(text, params);
    return result;
}

async function testConnection() {
    const result = await pool.query('SELECT NOW()');
    return result.rows[0];
}

module.exports = {
    pool,
    query,
    testConnection
};