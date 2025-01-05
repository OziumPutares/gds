async function hashPassword(password, salt) {
    const encoder = new TextEncoder();
    const saltedPassword = `${password}${salt}`;
    const data = encoder.encode(saltedPassword);

    const hashBuffer = await crypto.subtle.digest('SHA-256', data);
    const hashArray = Array.from(new Uint8Array(hashBuffer));
    console.log(hashArray.map(b => b.toString(16).padStart(2, '0')).join(''));
    return hashArray.map(b => b.toString(16).padStart(2, '0')).join('');
}

document.addEventListener('DOMContentLoaded', () => {
    const loginForm = document.getElementById('loginForm');
    const errorMessage = document.getElementById('error-message');
    const attemptsList = document.getElementById('login-attempts');
    const salt = "demofixed_salt"; // Replace with dynamic salt if needed

    loginForm.addEventListener('submit', async (e) => {
        e.preventDefault();

        const username = document.getElementById('username').value;
        const password = document.getElementById('password').value;

        try {
            const hashedPassword = await hashPassword(password, salt);

            const response = await fetch('/api/login', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    username,
                    hashedPassword,
                    salt
                })
            });

            const data = await response.json();

            if (data.success) {
                errorMessage.textContent = 'Login successful!';
                errorMessage.classList.remove('error');
                errorMessage.classList.add('success');
                errorMessage.classList.remove('hidden');

                const attemptItem = document.createElement('li');
                attemptItem.classList.add('success-attempt');
                attemptItem.textContent = `Success: ${username} - ${new Date().toLocaleTimeString()}`;
                if (attemptsList) {
                    attemptsList.prepend(attemptItem);
                }

                setTimeout(() => {
                    window.location.href = '/dashboard';
                }, 1500);
            } else {
                errorMessage.textContent = data.message;
                errorMessage.classList.add('error');
                errorMessage.classList.remove('success');
                errorMessage.classList.remove('hidden');

                const attemptItem = document.createElement('li');
                attemptItem.classList.add('failed-attempt');
                attemptItem.textContent = `Failed: ${username} - ${new Date().toLocaleTimeString()}`;
                if (attemptsList) {
                    attemptsList.prepend(attemptItem);
                }
            }
        } catch (error) {
            console.error('Login error:', error);
            errorMessage.textContent = `Error: ${error.message}`;
            errorMessage.classList.add('error');
            errorMessage.classList.remove('success');
            errorMessage.classList.remove('hidden');
        }
    });
});

